//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "game.h"
#include "shortid.h"
#include "utils.h"
#include <spdlog/spdlog.h>

#define INFO(msg) spdlog::info(msg)

quarantine_game::game::game(string host, double starting_money, string glitch_list, string map_name) : starting_money(
        starting_money), factory(glitch_factory(glitch_list)), game_map(map_factory::from_name(map_name)),
                                                                                                       glitch(glitch_factory::empty_glitch()) {
    id = shortid::get_new_id();
    turns = 0;
    has_started = false;
    can_roll_again = -1;
    redirect_to = -1;

    players.emplace_back(host, starting_money, 0, nullptr);
}

const string &quarantine_game::game::_id() const {
    return id;
}

void quarantine_game::game::start() {
    if (has_started) return;
    has_started = true;
    send_to_all(create_default_update());
}

const bool &quarantine_game::game::started() const {
    return has_started;
}

const uint32_t &quarantine_game::game::_turns() const {
    return turns;
}

bool quarantine_game::game::full() {
    return players.size() == 6;
}

const uint8_t quarantine_game::game::player_count() const {
    return players.size();
}

void quarantine_game::game::add_player(string name) {
    players.emplace_back(name, starting_money, 0, nullptr);
}

quarantine_game::player *quarantine_game::game::get_player(uint32_t turn) {
    if (turn >= players.size()) return nullptr;
    return &players[turn];
}

quarantine_game::player *quarantine_game::game::get_player(string name) {
    for (auto &it : players) if (it._name() == name) return &it;
    return nullptr;
}

uint8_t quarantine_game::game::get_player_turn(string name) {
    for (int i = 0; i < players.size(); i++) if (players[i]._name() == name) return i;
    return 7;
}

vector<quarantine_game::player> &quarantine_game::game::get_players() {
    return players;
}

quarantine_game::player *quarantine_game::game::get_offline_player(string name) {
    for (auto &it : offline_players) if (it->_name() == name) return it;
    return nullptr;
}

uint8_t quarantine_game::game::get_property_owner(uint8_t property) {
    auto box = game_map.from_id(property);

    if (!box.expired()) {
        return box.lock()->_owner();
    }
}

bool quarantine_game::game::is_property_owned(uint8_t property) {
    return get_property_owner(property) == 7;
}

void quarantine_game::game::roll_dice(uint8_t player) {
    using utils = quarantine_game::utils;
    if (turns % players.size() == player) {
        auto p = get_player(player);

        uint8_t previous = p->_position() + 0;

        uint8_t dice1 = 0, dice2 = 0;

        if (p->_turns_in_prison() == 0) {
            if (!goto_prison) {
                if (p->_money() >= 0) {
                    dice1 = utils::get_random_dice();
                    dice2 = utils::get_random_dice();
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
            dice1 = utils::get_random_dice();
            dice2 = utils::get_random_dice();

            auto f1 = [&](uint8_t p_turn, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant) -> json {
                return create_move_update(p_turn, dice1, dice2, new_pos, instant);
            };

            auto f2 = [&](string name) -> uint8_t {
                return get_player_turn(name);
            };

            quarantine_game::glitch_game_container container{&players, &redirect_to,
                                                             &can_roll_again, f1,
                                                             f2};

            if (dice1 != dice2 && p->_turns_in_prison() != 0) {
                glitch = factory.glitch(player, container);

                p->_turns_in_prison() = p->_turns_in_prison() - 1;

                json update = create_default_update();
                update["dice1"] = dice1;
                update["dice2"] = dice2;
                update = add_glitch_update(update);

                p->add_update(update);

                INFO("Player (name: " + p->_name() + ", turn: " + to_string(player) +
                     ") failed to escape prison. Dices: " + to_string(dice1) + ", " + to_string(dice2));
                move_player(player, p, dice1, dice2);
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

void quarantine_game::game::next_player(uint8_t p_turn) {
    auto player = get_player((turns + 1) % players.size());

    if (turns % players.size() != p_turn) return;

    if (get_offline_player(player->_name()) != nullptr || player->_blocked_for() > 0) {
        turns += 2;
        if (player->_blocked_for() > 0)
            player->_blocked_for() = player->_blocked_for() - 1;
    } else if (can_roll_again == p_turn)
        can_roll_again = -1;
    else
        turns++;

    send_to_all(create_default_update());
}

void quarantine_game::game::move_player(uint8_t p_turn, quarantine_game::player *player, uint8_t dice1, uint8_t dice2) {
    move_player(p_turn, player, dice1, dice2, (player->_position() + dice1 + dice2) % game_map.box_count(), false);
}

void quarantine_game::game::move_player(uint8_t p_turn, quarantine_game::player *player, uint8_t dice1, uint8_t dice2,
                                        uint8_t new_pos, bool instant) {
    player->_position() = new_pos;

    auto a = game_map[player->_position()];
    if (a.expired()) throw exception();
    else {
        auto b = dynamic_pointer_cast<property_box>(a.lock());

        if (b) {
            if (b->_owner() != 7 && b->_owner() != p_turn) {
                auto receiving = get_player(b->_owner());
                auto cost = 0;

                cost += b->_cost();
                cost += b->_houses() * 50;

                player->_money() -= cost;

                if (redirect_to == -1) {
                    receiving->_money() += cost;
                } else {
                    get_player(redirect_to)->_money() += cost;
                    redirect_to = -1;
                }
            }
        } else {
            auto f1 = [&](uint8_t p_turn, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant) -> json {
                return create_move_update(p_turn, dice1, dice2, new_pos, instant);
            };

            auto f2 = [&](string name) -> uint8_t {
                return get_player_turn(name);
            };

            quarantine_game::glitch_game_container container{&players, &redirect_to,
                                                             &can_roll_again, f1,
                                                             f2};

            if (game_map.is_glitch(player->_position())) {
                glitch = factory.glitch(p_turn, container);
            } else if (game_map.is_prison(player->_position())) {
                player->_turns_in_prison() = 3;
                glitch = factory.goto_prison(p_turn, container);
            }
        }

        json update = create_move_update(p_turn, dice1, dice2, (instant ? new_pos : dice1 + dice2), instant);

        for (auto &it : players) {
            if (glitch._title() == "empty" || *glitch.get_player().lock() != it)
                it.add_update(update);
            else {
                json custom = create_move_update(p_turn, dice1, dice2, (instant ? new_pos : dice1 + dice2),
                                                 instant);
                custom = add_glitch_update(custom);

                it.add_update(custom);
            }
        }
    }
}

void quarantine_game::game::buy_property(uint8_t property, uint8_t player) {
    if (turns % players.size() == player) {
        auto p = get_player(player);
        auto pr = game_map.from_id(property);

        if (pr.expired()) throw exception();
        else {
            auto pro = pr.lock();
            if (p->_position() == pro->_position()) {
                if (p->_money() < pro->_cost()) {
                    /*TODO Add error updates to web-interface*/

                } else {
                    p->_money() -= pro->_cost();
                    pro->_owner() = player;
                    send_to_all(create_color_update(property, player));
                }
            } else /*TODO Add error updates to web-interface*/;
        }
    }
}

void quarantine_game::game::buy_property(uint8_t property, string player) {
    auto p = get_player_turn(player);
    buy_property(property, p);
}

void quarantine_game::game::buy_house(uint8_t p_turn, uint8_t property, int8_t house_count) {
    if (turns % players.size() == p_turn) {
        auto p = get_player(p_turn);
        auto box = game_map.from_id(property);

        if (box.expired()) throw exception();
        else {
            auto a = box.lock();
            if (a->_owner() == p_turn) {
                //This doesn't check if the player has enough money to build the houses.
                //And it is intentional.

                if (house_count > a->_houses()) {
                    auto cost = (house_count - a->_houses()) * 50;

                    p->_money() -= cost;
                    a->_houses() = house_count;
                } else {
                    auto cost = (house_count - a->_houses()) * 25;

                    p->_money() += cost;
                    a->_houses() = house_count;
                }
                send_to_all(create_house_count_update(property, house_count));

            } else /*TODO Add error updates to web-interface*/ ;
        }
    }
}

void quarantine_game::game::player_quit(uint8_t player) {
    remove_player(player);

    auto properties = game_map.get_player_properties(player);
    std::map<uint8_t, uint8_t> to_old_format;


    for (auto &it : properties) {
        if (it.expired()) throw exception();
        else {
            auto a = it.lock();

            to_old_format.insert({a->_id(), a->_houses()});
        }
    }

    if (has_started) {
        send_to_all(create_player_quit_update(player, -1, to_old_format));
    } else {
        for (auto &p : players) {
            p.add_update(create_player_quit_update(player, get_player_turn(p._name()), to_old_format));
        }
    }
}

void quarantine_game::game::remove_player(uint8_t player) {
    auto p = get_player(player);

    if (p) {
        if (has_started) {
            game_map.delete_player_properties(player);

            offline_players.push_back(p);
        } else {
            players.erase(players.begin() + player);
        }
    }
}

void quarantine_game::game::player_rejoin(string player) {
    for (int i = 0; i < offline_players.size(); i++)
        if (offline_players[i]->_name() == player) offline_players.erase(offline_players.begin() + i);

    game_update_builder builder{{&players, &turns, &has_started}};
    game_update_builder builder1{{&players, &turns, &has_started}};
    builder.start();
    builder1.start();

    for (int i = 0; i < players.size(); i++) {
        auto p = players[i];

        builder.move(0, 0, p._position(), i, true);

        auto properties = game_map.get_player_properties(i);

        for (auto &it : properties) {
            if (it.expired()) throw exception();
            else {
                auto a = it.lock();
                builder.house_count(a->_id(), a->_houses());
                builder.color(a->_id(), i);
            }
        }
    }

    builder.other("hasStarted", has_started);
    builder.other_null("newImp");

    get_player(player)->add_update(builder.res());

    builder1.other("playerRejoined", ((uint64_t) get_player_turn(player)));
    builder1.other("hasStarted", has_started);


    for (int i = 0; i < players.size(); i++) {
        if (get_player_turn(player) == i) continue;
        players[i].add_update(builder1.res());
    }
}

json quarantine_game::game::create_default_update() {
    auto builder = new game_update_builder{{&players, &turns, &has_started}};

    builder->start()
            ->other_null("updatePlayerPos")
            ->other_null("updateHouseCount")
            ->other_null("newImp")
            ->other("hasStarted", has_started);

    auto res = builder->res();

    delete builder;

    return res;
}

json quarantine_game::game::create_move_update(uint8_t player, uint8_t dice1, uint8_t dice2, bool instant) {
    return create_move_update(dice1, dice2, dice1 + dice2, player, instant);
}

json
quarantine_game::game::create_move_update(uint8_t player, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant) {
    auto builder = new game_update_builder{{&players, &turns, &has_started}};

    builder->start()
            ->move(dice1, dice2, new_pos, player, instant)
            ->other_null("updateHouseCount")
            ->other_null("newImp")
            ->other("hasStarted", has_started);

    auto res = builder->res();

    delete builder;

    return res;
}

json quarantine_game::game::create_house_count_update(uint8_t property, uint8_t house_count) {
    auto builder = new game_update_builder{{&players, &turns, &has_started}};

    builder->start()
            ->house_count(property, house_count)
            ->other_null("updatePlayerPos")
            ->other_null("newGlitch")
            ->other("hasStarted", has_started);

    auto res = builder->res();

    delete builder;

    return res;
}

json quarantine_game::game::create_color_update(uint8_t property, uint8_t player) {
    auto builder = new game_update_builder{{&players, &turns, &has_started}};

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

json quarantine_game::game::create_glitch_response_update(uint8_t option_chosen) {
    if (glitch._message() != "empty") {
        glitch.choose_action(option_chosen);
        glitch = glitch_factory::empty_glitch();

        auto builder = new game_update_builder{{&players, &turns, &has_started}};

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

json quarantine_game::game::create_player_quit_update(uint8_t player_quit, int8_t player_to,
                                                      std::map<uint8_t, uint8_t> properties) {
    auto builder = new game_update_builder{{&players, &turns, &has_started}};

    builder->start()
            ->quit(player_quit, player_to)
            ->move(0, 0, -1, 0, true)
            ->other_null("newImp")
            ->other("hasStarted", has_started);

    for(auto keys = properties.begin(); keys != properties.end(); keys++) {
        builder->color(keys->first, -1)->house_count(keys->first, 0);
    }

    auto res = builder->res();

    delete builder;

    return res;
}

void quarantine_game::game::send_error_update(uint8_t player, string message) {
    json res = not_ok_status;
    res["error"] = message;

    players[player].add_update(res);
}

json quarantine_game::game::add_glitch_update(json update) {
    return add_glitch_update(update, glitch._title());
}

json quarantine_game::game::add_glitch_update(json update, string title) {
    game_update_builder builder {{&players, &turns, &has_started}, update};

    builder.new_glitch(glitch._message(), title, glitch._buttons());

    return nlohmann::json();
}

void quarantine_game::game::send_to_all(json update) {
    for(auto &player : players) {
        player.add_update(update);
    }
}


