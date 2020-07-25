//
// Created by Niccolò Mattei on 23/07/2020.
//
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "glitch.h"

#ifndef ELABORATO_ESAME_GLITCH_HANDLER_H
#define ELABORATO_ESAME_GLITCH_HANDLER_H

#define LAMBDA_FUNCTION_DECL [&]()


namespace quarantine_game {
    struct glitch_game_container {
        const vector<player> *players;
        int8_t *redirect_to;
        int8_t *can_roll_again;
        const function<json(uint8_t, uint8_t, uint8_t, uint8_t, bool)> create_move_update;
        const function<uint8_t(string)> get_player_turn;

        glitch_game_container(const vector<player> *players, int8_t *redirectTo, int8_t *canRollAgain,
                              const function<json(uint8_t, uint8_t, uint8_t, uint8_t, bool)> &createMoveUpdate,
                              const function<uint8_t(string)> &getPlayerTurn);
    };

    class glitch_handler {
    private:
        const static string glitch_folder;
    public:
        static void update_glitch_file(string id, string content);

        static json from_id(string id);

        static json from_name(string name);

        static vector<string> list_names();

        static json check_for_errors(json to_check);

        static json check_for_errors(json to_check, glitch_update_builder *builder);

        static json check_for_action_errors(int32_t glitch, string action, glitch_update_builder *builder);

        static vector<string> contains_player_reference(string message, uint8_t player_required);
    };

    class glitch_factory {
    private:
        json glitches;
        int32_t previous;
        glitch building;

        void parse_action(string action, glitch_game_container &state);
    public:
        explicit glitch_factory(string glitch_list);

        glitch glitch(uint8_t player, glitch_game_container &state);
        class glitch goto_prison(uint8_t player, glitch_game_container &state);
        static class glitch empty_glitch();

    };
}


#endif //ELABORATO_ESAME_GLITCH_HANDLER_H
