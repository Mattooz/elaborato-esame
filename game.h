#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include "player.h"
#include "glitch_handler.h"
#include "map_factory.h"


#ifndef ELABORATO_ESAME_GAME_H
#define ELABORATO_ESAME_GAME_H

using json = nlohmann::json;
using namespace std;


namespace quarantine_game {
    class game {
    private:
        int8_t can_roll_again;
        int8_t redirect_to;
        bool goto_prison;
        string id;
        double starting_money;
        uint32_t turns;
        vector<player> players;
        vector<player*> offline_players;
        bool has_started;
        glitch_factory factory;
        glitch glitch;
        quarantine_game::map game_map;

    public:
        game(string host, double starting_money, string glitch_list, string map_name);

        const string & _id() const;
        void start();
        const bool &started() const;
        const uint32_t &_turns() const;
        quarantine_game::map &map();
        bool full();
        const uint8_t player_count() const;
        void add_player(string name);
        player * get_player(uint32_t turn);
        player * get_player(string name);
        uint8_t get_player_turn(string name);
        vector<player> &get_players();
        player * get_offline_player(string name);
        uint8_t get_property_owner(uint8_t property);
        bool is_property_owned(uint8_t property);
        void roll_dice(uint8_t player);
        void next_player(uint8_t p_turn);
        void move_player(uint8_t p_turn, player * player, uint8_t dice1, uint8_t dice2);
        void move_player(uint8_t p_turn, player * player, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant);
        void buy_property(uint8_t property, uint8_t player);
        void buy_property(uint8_t property, string player);
        void buy_house(uint8_t p_turn, uint8_t property, int8_t house_count);
        void player_quit(uint8_t player);
        void remove_player(uint8_t player);
        void player_rejoin(string player);
        json create_default_update();
        json create_move_update(uint8_t player, uint8_t dice1, uint8_t dice2, bool instant);
        json create_move_update(uint8_t player, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant);
        json create_house_count_update(uint8_t property, uint8_t house_count);
        json create_color_update(uint8_t property, uint8_t player);
        json create_glitch_response_update(uint8_t option_chosen);
        json create_player_quit_update(uint8_t player_quit, int8_t player_to, std::map<uint8_t, uint8_t> properties);
        void send_error_update(uint8_t player, string message);
        json add_glitch_update(json update);
        json add_glitch_update(json update, string title);
        void send_to_all(json update);

        const static json not_ok_status;
        const static json ok_status;
    };
}


#endif //ELABORATO_ESAME_GAME_H
