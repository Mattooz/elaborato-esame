//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "game.h"
#include "shortid.h"
#include "utils.h"
#include <spdlog/spdlog.h>
#include <iostream>

#include <utility>

#define INFO(msg) spdlog::info(msg)

const nlohmann::json QuarantineGame::Game::not_ok_status = {{"ok", false}};
const nlohmann::json QuarantineGame::Game::ok_status = {{"ok", true}};
const uint8_t QuarantineGame::Game::not_found = 0xFF;

const string glitch_folder = "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/glitchlists";
const string map_folder = "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/maps";

QuarantineGame::Game::Game(const string &host, double starting_money, string glitch_list, string map_name)
        : starting_money(
        starting_money), factory(GlitchFactory(std::move(glitch_list))),
          game_map(MapFactory::from_name(std::move(map_name))),
          glitch(GlitchFactory::empty_glitch()) {

    if(game_map._map_id() == "not found") throw game_error("This is not a valid game map.");

    id = ShortId::get_new_id();
    turns = 0;
    has_started = false;
    can_roll_again = -1;
    redirect_to = -1;
    goto_prison = false;

    auto p = make_shared<Player>(host, starting_money, 0, nullptr);

    players.push_back(p);
}

const string &QuarantineGame::Game::_id() const {
    return id;
}

void QuarantineGame::Game::start() {
    if (has_started) return;
    has_started = true;
    send_to_all(create_default_update());
}

const bool &QuarantineGame::Game::started() const {
    return has_started;
}

const uint32_t &QuarantineGame::Game::_turns() const {
    return turns;
}

const bool QuarantineGame::Game::full() const {
    return players.size() == 6;
}

const QuarantineGame::Glitch &QuarantineGame::Game::current_glitch() const {
    return glitch;
}

int8_t &QuarantineGame::Game::redirect() {
    return redirect_to;
}

int8_t &QuarantineGame::Game::roll_again() {
    return can_roll_again;
}

QuarantineGame::Map &QuarantineGame::Game::map() {
    return game_map;
}


const uint8_t QuarantineGame::Game::player_count() const {
    return players.size();
}

void QuarantineGame::Game::add_player(string name) {
    auto p = make_shared<Player>(name, starting_money, 0, nullptr);

    players.push_back(p);
}

shared_ptr<QuarantineGame::Player> QuarantineGame::Game::get_player(uint32_t turn) const {
    if (turn >= players.size()) return nullptr;
    else {
        return players[turn];
    }
    return nullptr;
}

shared_ptr<QuarantineGame::Player> QuarantineGame::Game::get_player(string name) const {
    for (auto &it : players) if (it->_name() == name) return {it};
    return {};
}

const uint8_t QuarantineGame::Game::get_player_turn(string name) const {
    for (int i = 0; i < players.size(); i++) if (players[i]->_name() == name) return i;
    return Game::not_found;
}

vector<shared_ptr<QuarantineGame::Player>>& QuarantineGame::Game::get_players() {
    return players;
}

shared_ptr<QuarantineGame::Player> QuarantineGame::Game::get_offline_player(string name) const {
    for (auto &it : offline_players) {
        if (!it)
            throw QuarantineGame::game_error("Error while fetching offline_player. Pointer cannot be expired!");
        if (it->_name() == name) return it;
    }
    return {};
}

uint8_t QuarantineGame::Game::get_property_owner(uint8_t property) {
    auto box = game_map.from_id(property);

    if (box) {
        return box->_owner();
    }

    return Game::not_found;
}

void QuarantineGame::Game::roll_dice(uint8_t player) {
    if (turns % players.size() == player && has_started) {
        auto p = get_player(player);

        if (!p) throw game_error("Error while rolling dice. Player pointer is expired!");


        if (p->_blocked_for() > 0) return;

        uint8_t previous = p->_position() + 0;

        uint8_t dice1 = 0, dice2 = 0;

        if (p->_turns_in_prison() == 0) {
            if (!goto_prison) {
                if (p->_money() >= 0) {
                    dice1 = QuarantineGame::Utils::get_random_dice();
                    dice2 = QuarantineGame::Utils::get_random_dice();
                } else {
                    uint8_t distance = game_map.distance_to_next_glitch(p->_position());

                    dice1 = (distance / 2) + (distance % 2);
                    dice2 = (distance / 2);
                }
            } else {
                uint8_t distance = game_map.distance_to_prison(p->_position());

                if (distance >= 12) dice1 = dice2 = 6;
                else if (distance > 6) {
                    dice1 = 6;
                    dice2 = distance - 6;
                } else {
                    dice1 = distance - 6;
                    dice2 = 1;
                }
            }

            move_player(player, p, dice1, dice2);

            INFO("Player (name: " + p->_name() + ", turn: " + to_string(player) + ") moved to pos " +
                 to_string(p->_position()) + " (from: " + to_string(previous) + "). Dices: " + to_string(dice1) + ", " +
                 to_string(dice2));
        } else {
            dice1 = QuarantineGame::Utils::get_random_dice();
            dice2 = QuarantineGame::Utils::get_random_dice();

            auto f1 = [=](uint8_t p_turn, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant) -> json {
                return create_move_update(p_turn, dice1, dice2, new_pos, instant);
            };

            auto f2 = [=](string name) -> uint8_t {
                return get_player_turn(name);
            };

            QuarantineGame::GlitchGameContainer container{get_players(), &redirect_to,
                                                          &can_roll_again, f1,
                                                          f2};

            if (dice1 != dice2 && p->_turns_in_prison() > 0) {
                glitch = factory.glitch(player, container);

                p->_turns_in_prison() = p->_turns_in_prison() - 1;

                json update = create_default_update();
                update["dice1"] = dice1;
                update["dice2"] = dice2;

                for (auto &a : players) {
                    if (glitch.get_player()->_name() != a->_name())
                        a->add_update(update);
                }

                update = add_glitch_update(update);

                p->add_update(update);

                INFO("Player (name: " + p->_name() + ", turn: " + to_string(player) +
                     ") failed to escape prison. Dices: " + to_string(dice1) + ", " + to_string(dice2));
            } else {
                INFO("Player (name: " + p->_name() + ", turn: " + to_string(player) +
                     ") escaped prison and moved to pos " + to_string(p->_position()) + " (from: " +
                     to_string(previous) + "). Dices: " + to_string(dice1) + ", " + to_string(dice2));
                move_player(player, p, dice1, dice2);
                p->_turns_in_prison() = 0;
            }
        }
    }
}

void QuarantineGame::Game::next_player(uint8_t p_turn) {
    auto player = get_player((turns + 1) % players.size());
    if (!player) throw game_error("Error while advancing Player! Player pointer is expired!");

    if (turns % players.size() != p_turn || !has_started) return;

    if (get_offline_player(player->_name()) || player->_blocked_for() > 0) {
        turns += 2;
        if (player->_blocked_for() > 0)
            player->_blocked_for() = player->_blocked_for() - 1;
    } else if (can_roll_again == p_turn)
        can_roll_again = -1;
    else
        turns++;

    send_to_all(create_default_update());
}

void QuarantineGame::Game::move_player(uint8_t p_turn, const shared_ptr<QuarantineGame::Player> &player, uint8_t dice1,
                                       uint8_t dice2) {
    if (!player) throw game_error("Error while moving (overload) Player! Player pointer is expired!");
    move_player(p_turn, player, dice1, dice2, (player->_position() + dice1 + dice2) % game_map.box_count(),
                false);
}

void QuarantineGame::Game::move_player(uint8_t p_turn, shared_ptr<QuarantineGame::Player> player, uint8_t dice1,
                                       uint8_t dice2,
                                       uint8_t new_pos, bool instant) {
    if (turns % players.size() != p_turn || !has_started) return;
    if (!player) throw game_error("Error while moving Player! Player pointer is expired!");

    auto &ptr = player;

    ptr->_position() = new_pos;

    auto a = game_map[ptr->_position()];
    if (!a) throw exception();
    else {
        auto b = dynamic_pointer_cast<PropertyBox>(a);

        if (b) {
            if (b->_owner() != 7 && b->_owner() != p_turn) {

                auto receiving = get_player(b->_owner());
                auto cost = 0;

                cost += b->_cost();
                cost += b->_houses() * 50;

                if (redirect_to == -1) {
                    if (b->_owner() != Map::not_found) {
                        if (!receiving)
                            throw game_error("Error while transferring money! Receiving Player pointer is expired!");
                        ptr->_money() -= cost;
                        receiving->_money() += cost;
                    } else /*Do nothing*/;
                } else {
                    auto redirect = get_player(redirect_to);
                    if (!redirect)
                        throw game_error("Error while transferring money! Redirect-to Player pointer is expired!");
                    ptr->_money() -= cost;
                    redirect->_money() += cost;
                    redirect_to = -1;
                }
            }
        } else {
            auto f1 = [=](uint8_t p_turn, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant) -> json {
                return create_move_update(p_turn, dice1, dice2, new_pos, instant);
            };

            auto f2 = [=](string name) -> uint8_t {
                return get_player_turn(name);
            };

            QuarantineGame::GlitchGameContainer container{get_players(), &redirect_to,
                                                          &can_roll_again, f1,
                                                          f2};

            if (game_map.is_glitch(ptr->_position())) {

                if (ptr->_avoid() <= 0) {
                    glitch = factory.glitch(p_turn, container);
                    INFO("Player (name: " + ptr->_name() + ", turn: " + to_string(p_turn) +
                         ") arrived on a glitch box.");
                } else {
                    ptr->_avoid() -= 1;
                    INFO("Player (name: " + ptr->_name() + ", turn: " + to_string(p_turn) +
                         ") arrived on a glitch box but he is a lucky boy. Remaining avoid: " +
                         to_string(ptr->_avoid()));
                }
            } else if (game_map.is_prison(ptr->_position())) {
                INFO("Player (name: " + ptr->_name() + ", turn: " + to_string(p_turn) +
                     ") arrived on a goto-prison box.");
                ptr->_turns_in_prison() = 3;
                glitch = factory.goto_prison(p_turn, container);
            }
        }

        json update = create_move_update(p_turn, dice1, dice2, (instant ? new_pos : dice1 + dice2), instant);


        for (auto &it : players) {
            if (glitch._title() == "empty" || glitch.get_player()->_name() != it->_name())
                it->add_update(update);
            else {
                json custom = create_move_update(p_turn, dice1, dice2, (instant ? new_pos : dice1 + dice2),
                                                 instant);

                custom = add_glitch_update(custom);

                it->add_update(custom);
            }
        }
    }
}

void QuarantineGame::Game::buy_property(uint8_t property, uint8_t player) {
    if (turns % players.size() == player && has_started) {
        auto p = get_player(player);
        auto pro = game_map.from_id(property);

        if (!pro) throw game_error("Error while buying property! Property pointer is expired!");
        else if (!p) throw game_error("Error while buying property! Player pointer is expired!");
        else {
            if (p->_position() == pro->_position()) {
                if (p->_money() < pro->_cost()) {
                    INFO("Player (name: " + p->_name() + ", turn: " + to_string(player) +
                         ") failed to buy a property (name: " + pro->_name() + ", cost: " + to_string(pro->_cost()) +
                         ", id: " + to_string(pro->_id()) + ", pos: " + to_string(pro->_position()) + ")");

                } else {
                    INFO("Player (name: " + p->_name() + ", turn: " + to_string(player) +
                         ") just bought a property (name: " + pro->_name() + ", cost: " + to_string(pro->_cost()) +
                         ", id: " + to_string(pro->_id()) + ", pos: " + to_string(pro->_position()) + ")");
                    p->_money() -= pro->_cost();
                    pro->_owner() = player;
                    send_to_all(create_color_update(property, player));
                }
            }
        }
    }
}

void QuarantineGame::Game::buy_property(uint8_t property, string player) {
    auto p = get_player_turn(std::move(player));
    buy_property(property, p);
}

void QuarantineGame::Game::buy_house(uint8_t p_turn, uint8_t property, int8_t house_count) {
    if (turns % players.size() == p_turn && has_started) {
        auto p = get_player(p_turn);
        auto a = game_map.from_id(property);

        if (!a) throw game_error("Error while buying houses! Property pointer is expired!");
        else if (!p) throw game_error("Error while buying houses! Player pointer is expired!");
        else {
            if (a->_owner() == p_turn) {
                //This doesn't check if the Player has enough money to build the houses.
                //And it is intentional.

                if (house_count > a->_houses()) {
                    auto cost = (house_count - a->_houses()) * 50;

                    p->_money() -= cost;
                    a->_houses() = house_count;
                } else {
                    auto cost = (a->_houses() - house_count) * 25;

                    p->_money() += cost;
                    a->_houses() = house_count;
                }
                send_to_all(create_house_count_update(property, house_count));


                INFO("Player (name: " + p->_name() + ", turn: " + to_string(p_turn) +
                     ") just bought or sold some houses (new_count: " + to_string(house_count) +
                     ") on his property (name: " + a->_name() + ", cost: " + to_string(a->_cost()) +
                     ", id: " + to_string(a->_id()) + ", pos: " + to_string(a->_position()) + ")");
            }
        }
    }
}

void QuarantineGame::Game::player_quit(uint8_t player) {
    auto properties = game_map.get_player_properties(player);

    remove_player(player);

    if (turns % players.size() == player && has_started) {
        next_player(player);
    }

    std::map<uint8_t, uint8_t> to_old_format;

    for (auto &a : properties) {
        if (!a) throw game_error("Error while Player is quitting! Property pointer is expired!");
        else {

            to_old_format.insert({a->_id(), a->_houses()});
        }
    }

    if (has_started) {
        send_to_all(create_player_quit_update(player, -1, to_old_format));
    } else {
        for (auto &p : players) {
            p->add_update(create_player_quit_update(player, get_player_turn(p->_name()), to_old_format));
        }
    }
}

void QuarantineGame::Game::remove_player(uint8_t player) {
    auto p = get_player(player);

    if (p) {
        if (has_started) {
            game_map.delete_player_properties(player);


            offline_players.push_back(p);
        } else {
            players.erase(players.begin() + player);
        }
    } else throw game_error("Error while removing Player! Player pointer is expired!");
}

void QuarantineGame::Game::player_rejoin(string player) {
    if (!has_started) return;

    for (int i = 0; i < offline_players.size(); i++) {
        auto p = offline_players[i];
        if (!p) throw game_error("Error while Player is rejoining! Offline Player pointer is expired!");
        if (p->_name() == player) {
            offline_players.erase(offline_players.begin() + i);
            break;
        }
    }

    GameUpdateBuilder builder{{get_players(), &turns, &has_started, &game_map}};
    GameUpdateBuilder builder1{{get_players(), &turns, &has_started, &game_map}};
    builder.start();
    builder1.start();

    for (int i = 0; i < players.size(); i++) {
        auto p = players[i];

        builder.move(0, 0, p->_position(), i, true);

        auto properties = game_map.get_player_properties(i);

        for (auto &a : properties) {
            if (!a) throw game_error("Property pointer expired while player is rejoining!");
            else {
                builder.house_count(a->_id(), a->_houses());
                builder.color(a->_id(), i);
            }
        }
    }

    builder.other("hasStarted", has_started);

    auto pl = get_player(player);
    if (!pl) throw game_error("Error while Player is rejoining! Player pointer is expired!");
    pl->delete_updates();
    pl->add_update(builder.res());

    builder1.other("playerRejoined", ((uint64_t) get_player_turn(player)));
    builder1.other("hasStarted", has_started);


    for (int i = 0; i < players.size(); i++) {
        if (get_player_turn(player) == i) continue;
        players[i]->add_update(builder1.res());
    }
}

json QuarantineGame::Game::create_default_update() {
    auto builder = new GameUpdateBuilder{{get_players(), &turns, &has_started, &game_map}};

    builder->start()
            ->other_null("updatePlayerPos")
            ->other_null("updateHouseCount")
            ->other("hasStarted", has_started);

    auto res = builder->res();

    delete builder;

    return res;
}

QuarantineGame::GlitchGameContainer QuarantineGame::Game::get_game_container() {

    auto f1 = [=](uint8_t p_turn, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant) -> json {
        return create_move_update(p_turn, dice1, dice2, new_pos, instant);
    };

    auto f2 = [=](string name) -> uint8_t {
        return get_player_turn(name);
    };

    QuarantineGame::GlitchGameContainer container{get_players(), &redirect_to,
                                                  &can_roll_again, f1,
                                                  f2};

    return container;
}

json
QuarantineGame::Game::create_move_update(uint8_t player, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant) {
    auto builder = new GameUpdateBuilder{{get_players(), &turns, &has_started, &game_map}};

    builder->start()
            ->move(dice1, dice2, new_pos, player, instant)
            ->other_null("updateHouseCount")
            ->other("hasStarted", has_started);

    auto res = builder->res();

    delete builder;

    return res;
}

json QuarantineGame::Game::create_house_count_update(uint8_t property, uint8_t house_count) {
    auto builder = new GameUpdateBuilder{{get_players(), &turns, &has_started, &game_map}};

    builder->start()
            ->house_count(property, house_count)
            ->other_null("updatePlayerPos")
            ->other_null("newGlitch")
            ->other("hasStarted", has_started);

    auto res = builder->res();

    delete builder;

    return res;
}

json QuarantineGame::Game::create_color_update(uint8_t property, uint8_t player) {
    auto builder = new GameUpdateBuilder{{get_players(), &turns, &has_started, &game_map}};

    builder->start()
            ->color(property, player)
            ->other_null("updatePlayerPos")
            ->other_null("updateHouseCount")
            ->other_null("newGlitch")
            ->other("hasStarted", has_started);

    auto res = builder->res();

    delete builder;

    return res;
}

json QuarantineGame::Game::create_glitch_response_update(uint8_t option_chosen) {
    if (glitch._message() != "empty") {
        INFO("Player (name: " + glitch.get_player()->_name() + ", turn: " +
             to_string(get_player_turn(glitch.get_player()->_name())) + ") just chose action (" +
             to_string(option_chosen) + ")");
        glitch.choose_action(option_chosen);
        glitch = GlitchFactory::empty_glitch();

        auto builder = new GameUpdateBuilder{{get_players(), &turns, &has_started, &game_map}};

        builder->start()
                ->other_null("updatePlayerPos")
                ->other_null("updateHouseCount")
                ->other_null("newGlitch")
                ->other("hasStarted", has_started)
                ->other("lockMainButtons", false);


        auto res = builder->res();

        delete builder;

        return res;
    } else return {};
}

json QuarantineGame::Game::create_player_quit_update(uint8_t player_quit, int8_t player_to,
                                                     std::map<uint8_t, uint8_t> properties) {
    auto builder = new GameUpdateBuilder{{get_players(), &turns, &has_started, &game_map}};

    builder->start()
            ->quit(player_quit, player_to)
            ->move(0, 0, -1, 0, true)
            ->other_null("newGlitch")
            ->other("hasStarted", has_started);

    for (auto keys = properties.begin(); keys != properties.end(); keys++) {
        builder->color(keys->first, -1)->house_count(keys->first, 0);
    }

    auto res = builder->res();

    delete builder;

    return res;
}

void QuarantineGame::Game::send_error_update(uint8_t player, string message) {
    json res = not_ok_status;
    res["error"] = message;

    players[player]->add_update(res);
}

json QuarantineGame::Game::add_glitch_update(json update) {
    return add_glitch_update(std::move(update), glitch._title());
}

json QuarantineGame::Game::add_glitch_update(json update, string title) {
    GameUpdateBuilder builder{{get_players(), &turns, &has_started, &game_map}, std::move(update)};

    builder.new_glitch(glitch._message(), std::move(title), glitch._buttons());

    return builder.res();
}

void QuarantineGame::Game::send_to_all(const json &update) {
    for (auto &player : players) {
        player->add_update(update);
    }
}

