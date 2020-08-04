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
#define EMPTY_LAMBDA [=](){}

class Glitch_factory_fixture;

namespace quarantine_game {
    struct GlitchGameContainer {
        vector<weak_ptr<Player>> players;
        int8_t *redirect_to;
        int8_t *can_roll_again;
        const function<json(uint8_t, uint8_t, uint8_t, uint8_t, bool)> create_move_update;
        const function<uint8_t(string)> get_player_turn;

        GlitchGameContainer(vector<weak_ptr<Player>> players, int8_t *redirectTo, int8_t *canRollAgain,
                            function<json(uint8_t, uint8_t, uint8_t, uint8_t, bool)> createMoveUpdate,
                            function<uint8_t(string)> getPlayerTurn);
    };

    class GlitchHandler {
    private:
        const static string glitch_folder;
    public:
        static void update_glitch_file(string id, string content);

        static json from_id(string id);

        static json from_name(string name);

        static vector<string> list_names();

        static vector<json> lists();

        static json check_for_errors(json to_check);

        static json check_for_errors(json to_check, GlitchUpdateBuilder *builder);

        static json check_for_action_errors(int32_t glitch, string action, GlitchUpdateBuilder *builder);

        static vector<string> contains_player_reference(string message, uint8_t player_required);
    };

    class GlitchFactory {
    private:
        json glitches;
        int32_t previous;
        Glitch building;

        void parse_action(string action, GlitchGameContainer &state);
        uint8_t get_random_player(uint8_t p_turn, quarantine_game::GlitchGameContainer &state);
        json get_random_glitch(GlitchGameContainer & state);
        uint32_t get_random_num();

        friend class ::Glitch_factory_fixture;
    public:
        explicit GlitchFactory(string glitch_list);

        Glitch glitch(uint8_t player, GlitchGameContainer &state);
        class Glitch goto_prison(uint8_t player, GlitchGameContainer &state);
        static class Glitch empty_glitch();


    };
}


#endif //ELABORATO_ESAME_GLITCH_HANDLER_H
