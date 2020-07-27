//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "glitch_handler.h"
#include "glitch.h"
#include "utils.h"
#include <cmath>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <spdlog/spdlog.h>

#define PLAYERS (*state.players)
#define ERROR(msg) spdlog::error(msg)


quarantine_game::glitch_game_container::glitch_game_container(vector<player> *players, int8_t *redirectTo,
                                                              int8_t *canRollAgain,
                                                              const function<json(uint8_t, uint8_t, uint8_t, uint8_t,
                                                                                  bool)> &createMoveUpdate,
                                                              const function<uint8_t(string)> &getPlayerTurn) : players(
        players), redirect_to(redirectTo), can_roll_again(canRollAgain), create_move_update(createMoveUpdate),
                                                                                                                get_player_turn(
                                                                                                                        getPlayerTurn) {}

quarantine_game::glitch_factory::glitch_factory(string glitch_list) : building(empty_glitch()),
                                                                      glitches(glitch_handler::from_name(glitch_list)),
                                                                      previous(-1) {}

quarantine_game::glitch quarantine_game::glitch_factory::empty_glitch() {
    return quarantine_game::glitch("empty", "empty", {}, {}, 0);
}

void quarantine_game::glitch_factory::parse_action(string action, quarantine_game::glitch_game_container &state) {
    if (action.empty()) {
        class action ac;
        ac + LAMBDA_FUNCTION_DECL {};
        building.actions.push_back(ac);
    }

    auto split_comma = utils::split(action, ",");

    for (auto a : split_comma) {
        if (a.empty()) continue;

        auto split_and = utils::split(a, "and");

        class action ac;
        for (auto b : split_and) {
            boost::algorithm::trim(b);
            if (b.empty()) continue;
            auto split_space = utils::split(b, " ");


            auto &c = split_space[0];
            if (c == "pay") {
                if (split_space[1] == "rnd") {
                    int32_t rnd = utils::get_random_num(51);

                    ac + LAMBDA_FUNCTION_DECL {
                        int32_t cost = abs((building.get_player()->_money() / 100) * rnd);

                        building.get_player()->_money() -= cost;
                    };
                    boost::replace_all(building.message, "<money>", to_string(rnd) + "%");
                } else {
                    double percent = stod(split_space[1]);
                    if (percent <= 1) {
                        ac + LAMBDA_FUNCTION_DECL {
                            int32_t cost = abs(building.get_player()->_money() * percent);

                            building.get_player()->_money() += cost;
                        };
                        boost::replace_all(building.message, "<money>", to_string(percent) + "%");
                    } else {
                        ac + LAMBDA_FUNCTION_DECL {
                            building.get_player()->_money() += percent;
                        };
                        boost::replace_all(building.message, "<money>", to_string(percent));
                    }
                }
            } else if (c == "cash-in") {
                if (split_space[1] == "rnd") {
                    int32_t rnd = utils::get_random_num(51);

                    ac + LAMBDA_FUNCTION_DECL {
                        int32_t cost = abs((building.get_player()->_money() / 100) * rnd);

                        building.get_player()->_money() += cost;
                    };
                    boost::replace_all(building.message, "<money>", to_string(rnd) + "%");
                } else {
                    double percent = stod(split_space[1]);
                    if (percent <= 1) {
                        ac + LAMBDA_FUNCTION_DECL {
                            int32_t cost = abs(building.get_player()->_money() * percent);

                            building.get_player()->_money() += cost;
                        };
                        boost::replace_all(building.message, "<money>", to_string(percent) + "%");
                    } else {
                        ac + LAMBDA_FUNCTION_DECL {
                            building.get_player()->_money() += percent;
                        };
                        boost::replace_all(building.message, "<money>", to_string(percent));
                    }
                }
            } else if (c == "get-transaction") {
                ac + LAMBDA_FUNCTION_DECL {
                    *state.redirect_to = state.get_player_turn(building.get_player()->_name());
                };
            } else if (c == "roll-again") {
                ac + LAMBDA_FUNCTION_DECL {
                    *state.can_roll_again = state.get_player_turn(building.get_player()->_name());
                };
            } else if (c == "avoid") {
                int8_t avoid = stoi(split_space[1]);
                ac + LAMBDA_FUNCTION_DECL {
                    building.get_player()->_avoid() = avoid;
                };
            } else if (c == "stop") {
                int8_t stop = stoi(split_space[1]);
                ac + LAMBDA_FUNCTION_DECL {
                    building.get_player()->_blocked_for() = stop;
                };
            } else if (c == "ok") {
                ac + LAMBDA_FUNCTION_DECL {};
            }
        }
        building.actions.push_back(ac);
    }
}


quarantine_game::glitch
quarantine_game::glitch_factory::glitch(uint8_t player, quarantine_game::glitch_game_container &state) {
    building = empty_glitch();

    json random_glitch = get_random_glitch(state);

    building.message = random_glitch["message"];
    building.title = "IMPREVISTO";
    building.requires = random_glitch["requires"];
    building.required.push_back(&PLAYERS[player]);

    boost::replace_all(building.message, "<player_1>", building.required[0]->_name());

    for(int i = 1; i < building.requires; i++) {
        uint8_t rnd_pl = get_random_player(player, state);
        building.required.push_back(&PLAYERS[rnd_pl]);
    }

    parse_action(random_glitch["action"], state);

    for(auto &it : random_glitch["buttons"]) building.buttons.push_back(it);
    
    return building;
}

quarantine_game::glitch
quarantine_game::glitch_factory::goto_prison(uint8_t player, quarantine_game::glitch_game_container &state) {
    vector<action> actions;
    vector<string> buttons;

    action ac;

    ac + LAMBDA_FUNCTION_DECL {
        json update = state.create_move_update(player, 1, 1, 9, true);
        for (auto it : PLAYERS) {
            it.add_update(update);
        }

        PLAYERS[player]._position() = 9;
    };

    actions.push_back(ac);
    buttons.emplace_back("Entra in prigione");

    class glitch gl{
            "Sei finito sulla casella \"vai in prigione\". Passerai i prossimi 3 turni fermo in prigione a meno che tu non riesca ad uscirne tirando due dadi uguali",
            "VAI IN PRIGIONE", actions, buttons, 1};

    gl.required.push_back(&PLAYERS[player]);

    return gl;
}

uint8_t
quarantine_game::glitch_factory::get_random_player(uint8_t p_turn, quarantine_game::glitch_game_container &state) {
    if (PLAYERS.size() == 1) return 0xFF;

    uint8_t res = utils::get_random_num(PLAYERS.size());
    if (res == p_turn) return get_random_player(p_turn, state);

    return res;
}

json quarantine_game::glitch_factory::get_random_glitch(quarantine_game::glitch_game_container &state) {
    json res = glitches.at(get_random_num());

    if (res["requires"] > PLAYERS.size()) return get_random_glitch(state);
    return res;
}

uint32_t quarantine_game::glitch_factory::get_random_num() {
    uint32_t res = utils::get_random_num(glitches.size());

    if (res == previous) return get_random_num();
    else previous = res;

    return res;
}

void quarantine_game::glitch_handler::update_glitch_file(string id, string content) {

}

json quarantine_game::glitch_handler::from_id(string id) {
    return nlohmann::json();
}

json quarantine_game::glitch_handler::from_name(string name) {
    return nlohmann::json();
}

vector<string> quarantine_game::glitch_handler::list_names() {
    return vector<string>();
}

json quarantine_game::glitch_handler::check_for_errors(json to_check) {
    return nlohmann::json();
}

json quarantine_game::glitch_handler::check_for_errors(json to_check, quarantine_game::glitch_update_builder *builder) {
    return nlohmann::json();
}

json quarantine_game::glitch_handler::check_for_action_errors(int32_t glitch, string action,
                                                              quarantine_game::glitch_update_builder *builder) {
    return nlohmann::json();
}

vector<string> quarantine_game::glitch_handler::contains_player_reference(string message, uint8_t player_required) {
    return vector<string>();
}
