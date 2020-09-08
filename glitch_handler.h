//
// Created by Niccolò Mattei on 23/07/2020.
//
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "glitch.h"

#ifndef ELABORATO_ESAME_GLITCH_HANDLER_H
#define ELABORATO_ESAME_GLITCH_HANDLER_H

#define LAMBDA_FUNCTION_DECL [=]()

class Glitch_factory_suite;

namespace QuarantineGame {
    struct GlitchGameContainer {
        vector<shared_ptr<Player>> players;
        int8_t *redirect_to;
        int8_t *can_roll_again;
        const function<json(uint8_t, uint8_t, uint8_t, uint8_t, bool)> create_move_update;
        const function<uint8_t(string)> get_player_turn;

        GlitchGameContainer(vector<shared_ptr<Player>> players, int8_t *redirectTo, int8_t *canRollAgain,
                            function<json(uint8_t, uint8_t, uint8_t, uint8_t, bool)> createMoveUpdate,
                            function<uint8_t(string)> getPlayerTurn);
    };

    namespace GlitchHandler {
        void update_glitch_file(string id, string content);

        json from_id(string id);

        json from_name(string name);

        vector<string> list_names();

        vector<json> lists();

        json check_for_errors(json to_check);

        json check_for_errors(json to_check, GlitchUpdateBuilder *builder);

        void check_for_action_errors(int32_t glitch, string action, GlitchUpdateBuilder *builder);

        vector<string> contains_player_reference(string message, uint8_t player_required);
    };

    class GlitchFactory {
    private:
        friend class ::Glitch_factory_suite;

        json glitches;
        int32_t previous;
        Glitch building;

        void parse_action(string action, GlitchGameContainer &state);

        uint8_t get_random_player(uint8_t p_turn, QuarantineGame::GlitchGameContainer &state);

        json get_random_glitch(GlitchGameContainer &state);

        json get_glitch(int32_t which);

        uint32_t get_random_num();

    public:
        explicit GlitchFactory(string glitch_list);

        Glitch glitch(uint8_t player, GlitchGameContainer &state, int32_t which = -1);

        class Glitch goto_prison(uint8_t player, GlitchGameContainer &state);

        static class Glitch empty_glitch();
    };
}


#endif //ELABORATO_ESAME_GLITCH_HANDLER_H
