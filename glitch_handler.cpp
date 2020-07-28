//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "glitch_handler.h"
#include "glitch.h"
#include "utils.h"
#include "shortid.h"
#include "update_builder.h"
#include <cmath>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>

#define PLAYERS (*state.players)
#define ERROR(msg) spdlog::error(msg)

using namespace std::filesystem;


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
            } else {
                ERROR("Unknown command!");
                ac + EMPTY_LAMBDA;
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

    for (int i = 1; i < building.requires; i++) {
        uint8_t rnd_pl = get_random_player(player, state);
        building.required.push_back(&PLAYERS[rnd_pl]);
    }

    parse_action(random_glitch["action"], state);

    for (auto &it : random_glitch["buttons"]) building.buttons.push_back(it);

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


const std::string quarantine_game::glitch_handler::glitch_folder = "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/glitchlists";

void quarantine_game::glitch_handler::update_glitch_file(string id, string content) {
    ofstream of{glitch_folder + "/" + id + ".json", ofstream::trunc};
    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    of.write((char *) bom, sizeof(bom));
    of << content;
    of.close();
}

json quarantine_game::glitch_handler::from_id(string id) {
    for (auto &it : lists()) if (it["id"] == id) return it["glitches"];
    return nlohmann::json();
}

json quarantine_game::glitch_handler::from_name(string name) {
    for (auto &it : lists()) if (it["name"] == name) return it["glitches"];
    return nlohmann::json();
}

vector<string> quarantine_game::glitch_handler::list_names() {
    vector<string> names;

    for (auto &it : lists()) {
        names.push_back(it["name"]);
    }

    return names;
}

vector<json> quarantine_game::glitch_handler::lists() {
    vector<json> res;

    for (auto &it : directory_iterator(glitch_handler::glitch_folder)) {
        string s = utils::read_file(it.path());
        json read = json::parse(s);

        if (!shortid::is_id(it.path().stem().string())) continue;
        read["id"] = it.path().stem().string();

        res.push_back(read);
    }

    return res;
}

json quarantine_game::glitch_handler::check_for_errors(json to_check) {
    glitch_update_builder builder;
    return check_for_errors(to_check, &builder);
}

json quarantine_game::glitch_handler::check_for_errors(json to_check, quarantine_game::glitch_update_builder *builder) {
    if (!to_check.contains("name"))
        builder->glitch_error("Nome della lista non presente all'interno della richiesta", -1);
    else {
        try {
            auto a = to_check["name"].get<string>();

            if (a.empty()) {
                builder->glitch_error("Il nome della lista è vuoto", -1);
            }
        } catch (exception &e) {
            builder->glitch_error("Il nome della lista non è una stringa di testo", -1);
        }
    }

    json glt;

    if (!to_check.contains("glitches")) {
        builder->glitch_error("La lista degli imprevisti non è stata trovata", -1);
        return builder->res();
    } else if (!to_check["glitches"].is_array()) {
        builder->glitch_error("La lista degli imprevisti deve essere una JSON array", -1);
        return builder->res();
    } else if (to_check["glitches"].size() < 6)
        builder->glitch_error(
                "La lista degli imprevisti contiene 5 o meno elementi. Questo rende il gioco monotono dopo poco tempo",
                -1);
    else if (to_check["glitches"].size() < 11)
        builder->glitch_warning(
                "La lista degli imprevisti contiene 10 o meno elementi. Questo potrebbere rendere il gioco monotono dopo poco tempo",
                -1);

    glt = to_check["glitches"];

    for (int i = 0; i < glt.size(); i++) {
        json a;

        try {
            a = glt[i].get<json>();
        } catch (exception &e) {
            builder->glitch_error("Questo imprevisto non è in formato JSON", i);
            continue;
        }

        int requires = -1;
        string message;
        vector<string> buttons;
        string action;

        if (!a.contains("requires"))
            builder->glitch_error("In questo imprevisto non è presente il campo dei giocatori richiesti", i);
        else {
            try {
                requires = a["requires"].get<int>();
            } catch (exception &e) {
                builder->glitch_error("Il campo dei giocatori richiesti non è un numero intero", i);
                continue;
            }

            if (requires > 6 || requires < 1) {
                builder->glitch_error("L'imprevisto può coinvolgere solo tra gli 1 e i 6 giocatori", i);
                continue;
            }
        }

        if (!a.contains("message"))
            builder->glitch_warning("In questo imprevisto non è presente il campo del testo", i);
        else {
            try {
                message = a["message"].get<string>();
            } catch (exception &e) {
                builder->glitch_error("Il testo di questo imprevisto non è una stringa di testo", i);
                continue;
            }

            if (message.empty()) {
                builder->glitch_error("Il testo di questo imprevisto", i);
                continue;
            }
        }

        if (!message.empty() && requires > 1) {
            auto cnt = contains_player_reference(message, requires);

            if (!cnt.empty()) {
                string warn_msg = "Il testo di questo imprevisto con più di un di un giocatore non contiene riferimenti ad alcuni dei giocatori! Riferimenti mancanti: ";
                for (int l = 0; l < cnt.size(); l++) {
                    warn_msg += cnt[l];
                    if (l != (cnt.size() - 1)) warn_msg += ", ";
                }

                builder->glitch_warning(warn_msg, i);
            }
        }

        if (!a.contains("buttons"))
            builder->glitch_error("Questo imprevisto non contiene la lista dei bottoni", i);
        else if (!a.is_array())
            builder->glitch_error("La lista di bottoni di questo imprevisto è in un formato errato.", i);
        else {
            try {
                for (auto b : a["buttons"]) {
                    if (b.get<string>().empty()) {
                        builder->glitch_error("Uno dei bottoni di questo imprevisto non ha testo.", i);
                    } else
                        buttons.push_back(b.get<string>());
                }
            } catch (exception &e) {
                builder->glitch_error("La lista dei bottoni deve contenere solo linee di testo", i);
                continue;
            }
        }

        if (!a.contains("action"))
            builder->glitch_error("Questo imprevisto non contiene le azioni dei rispettivi bottoni", i);
        else {
            try {
                action = a["action"].get<string>();
            } catch (exception &e) {
                builder->glitch_error("Le azioni di questo imprevisto sono in un formato sconosciuto", i);
                continue;
            }

            if (action.empty())
                builder->glitch_error("L'azione di questo imprevisto è vuota.", i);
        }

        if (!buttons.empty() && !action.empty()) {
            if (buttons.size() != utils::split(action, ",").size()) {
                builder->glitch_error("C'è una discrepanza tra il numero di bottoni e quello delle azioni", i);
                continue;
            } else {
                if (buttons.size() > 2)
                    builder->glitch_warning(
                            "In questo imprevisto ci sono più di 2 bottoni. Dal testing è stato notato che più di 2 bottoni che più di due bottoni possono creare problemi grafici nell'interfaccia",
                            i);
                if (!message.empty()) {
                    if ((action.find("pay") != string::npos || action.find("cash-in") != string::npos) &&
                        message.find("<money>") == string::npos)
                        builder->glitch_warning(
                                "Il testo dell'imprevisto non contiene alcun riferimento a quanti soldi verranno dati/tolti al giocatore",
                                i);
                }
                check_for_action_errors(i, action, builder);
            }
        }

    }
    return nlohmann::json();
}

json quarantine_game::glitch_handler::check_for_action_errors(int32_t glitch, string action,
                                                              quarantine_game::glitch_update_builder *builder) {

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
                if (split_space.size() != 2)
                    builder->glitch_error("'pay' richiede <b>solo</b> un argomento", glitch);
                else if (!(split_space[1] == "rnd") && !utils::is_number(split_space[1]))
                    builder->glitch_error(
                            "Il comando 'pay' accetta solo un numero o il tag 'rnd' come argomento. Il tuo parametro: " +
                            split_space[1],
                            glitch);
            } else if (c == "cash-in") {
                if (split_space.size() != 2)
                    builder->glitch_error("'cash-in' richiede <b>solo</b> un argomento", glitch);
                else if (!(split_space[1] == "rnd") && !utils::is_number(split_space[1]))
                    builder->glitch_error(
                            "Il comando 'cash-in' accetta solo un numero o il tag 'rnd' come argomento. Il tuo parametro: " +
                            split_space[1],
                            glitch);
            } else if (c == "get-transaction") {
                //do nothing, nothing to check
            } else if (c == "roll-again") {
                //do nothing, nothing to check
            } else if (c == "avoid") {
                if (split_space.size() != 2)
                    builder->glitch_error("'avoid' richiede <b>solo</b> un argomento", glitch);
                else if (!utils::is_integer(split_space[1]))
                    builder->glitch_error(
                            "Il comando 'avoid' accetta solo un numero intero. Il tuo parametro: " +
                            split_space[1],
                            glitch);

            } else if (c == "stop") {
                if (split_space.size() != 2)
                    builder->glitch_error("'stop' richiede <b>solo</b> un argomento", glitch);
                else if (!utils::is_integer(split_space[1]))
                    builder->glitch_error(
                            "Il comando 'stop' accetta solo un numero intero. Il tuo parametro: " +
                            split_space[1],
                            glitch);

            } else if (c == "ok") {
                //do nothing, nothing to check
            } else {
                builder->glitch_error("Questo comando è sconosciuto.", glitch);
            }
        }
    }
}

vector<string> quarantine_game::glitch_handler::contains_player_reference(string message, uint8_t player_required) {
    vector<string> res;

    for(int i = 0; i < player_required; i++) {
        string cnt = "<player_" + to_string(i + 1) + ">";

        if(message.find(cnt) == string::npos) res.push_back("Giocatore" + to_string(i + 1));
    }

    return res;
}


