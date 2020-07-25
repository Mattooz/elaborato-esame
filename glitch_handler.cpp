//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "glitch_handler.h"

quarantine_game::glitch_game_container::glitch_game_container(const vector<player> *players, int8_t *redirectTo,
                                                              int8_t *canRollAgain,
                                                              const function<json(uint8_t, uint8_t, uint8_t, uint8_t,
                                                                                  bool)> &createMoveUpdate,
                                                              const function<uint8_t(string)> &getPlayerTurn) : players(
        players), redirect_to(redirectTo), can_roll_again(canRollAgain), create_move_update(createMoveUpdate),
                                                                                                                get_player_turn(
                                                                                                                        getPlayerTurn) {}

quarantine_game::glitch quarantine_game::glitch_factory::empty_glitch() {
    return quarantine_game::glitch("empty", "empty", {}, {});
}
