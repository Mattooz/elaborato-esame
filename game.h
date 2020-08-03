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

class Glitch_factory_fixture;

namespace quarantine_game {

    /**
     * The game class handles almost all of the logic (together with the web_server) of the game itself.
     */
    class Game {
    private:
        const static uint8_t not_found = 0xFF;
        int8_t can_roll_again;
        int8_t redirect_to;
        bool goto_prison;
        string id;
        double starting_money;
        uint32_t turns;
        vector<shared_ptr<Player>> players;
        vector<weak_ptr<Player>> offline_players;
        bool has_started;
        GlitchFactory factory;
        Glitch glitch;
        quarantine_game::Map game_map;

        friend class ::Glitch_factory_fixture;

    public:
        Game(const string& host, double starting_money, string glitch_list, string map_name);

        /**
         * Returns the id of the game.
         *
         * @return the id
         */
        const string &_id() const;

        /**
         * Starts the game and sends a default update to all the players.
         */
        void start();

        /**
         * Checks if the game is started or not.
         *
         * @return true or false
         */
        const bool &started() const;

        /**
         * Returns the total turns number.
         *
         * @return the number of turns
         */
        const uint32_t &_turns() const;

        /**
         * Returns the current game map.
         *
         * @return a map
         */
        quarantine_game::Map &map();

        /**
         * Checks if the game is full or not. The max amount of players is 6.
         *
         * @return true or false
         */
        const bool full() const;

        /**
         * Returns the current amount of players present in the game.
         *
         * @return the number of players
         */
        const uint8_t player_count() const;

        /**
         * Adds a new player to the game.
         *
         * @param name the name of the player
         */
        void add_player(string name);

        /**
         * Finds a player from its turn in the game. If it finds the player it returns a shared_ptr made from a weak_ptr
         * using weak_ptr.lock(). If no player is found it returns a null shared_ptr.
         *
         * @param turn the turn of the player
         * @return a shared_ptr containing the player.
         */
        weak_ptr<Player> get_player(uint32_t turn);

        /**
         * Finds a player from its name in the game. If it finds the player it returns a shared_ptr made from a weak_ptr
         * using lock(). If no player is found it returns a null shared_ptr.
         *
         * @param turn the turn of the player
         * @return a shared_ptr containing the player.
         */
        weak_ptr<Player> get_player(string name);

        /**
         * Finds the player turn from the given name inside of the game. If no player is found it returns
         * @see game::not_found.
         *
         * @param name the name of the player
         * @return the turn
         */
        const uint8_t get_player_turn(string name) const;

        /**
         * Returns a copy of the player list made from weak pointers. If any of these pointers is expired for whatever
         * reason it throws a game_error.
         *
         * @return a copy list of the players
         */
        vector<weak_ptr<Player>> get_players();

        /**
         * Returns an offline player from the offline players vector. If no player is found it returns a null
         * shared_ptr.
         *
         * @param name the name of the offline player
         * @return a shared_ptr containing the offline_player
         */
        weak_ptr<quarantine_game::Player> get_offline_player(string name);

        /**
         * Finds the owner of a given property. If the property is not found or it has no owner it returns
         * @see game::not_found.
         *
         * @param property the property id.
         * @return the owner or @see game::not_found
         */
        uint8_t get_property_owner(uint8_t property);

        /**
         * Checks if the property is owned or not.
         *
         * @param property the property id.
         * @return true or false
         */
        bool is_property_owned(uint8_t property);

        /**
         * Rolls the dices for a given a player. It checks various factors and chooses the
         * next action accordingly. For example if the player is in prison and the two dice match it will move the
         * player forward, otherwise it will give the player a glitch.
         *
         * @param player the player turn. If the player turn does not match the actual turns stored in the game it will return.
         */
        void roll_dice(uint8_t player);

        /**
         * Advances by a number between 0 and 2 the turns of the game based on various factors, for example if the
         * player is blocked it will advance by 2.
         *
         * @param p_turn the player turn. If the player turn does not match the actual turns stored in the game it will return.
         */
        void next_player(uint8_t p_turn);

        /**
         * Moves the player by the sum of the two dices. Overloaded function. @see game::move_player.
         *
         * @param p_turn the player turn.
         * @param player the actual player object.
         * @param dice1 the first dice rolled.
         * @param dice2 the second dice rolled.
         */
        void move_player(uint8_t p_turn, const weak_ptr<Player>& player, uint8_t dice1, uint8_t dice2);

        /**
         * Moves the player. If the movement is instant the new_pos param indicates the actual new position of the
         * player, otherwise it should be the sum of the two dices.
         * Checks also where the player has arrived. If, for example, the player has arrived on an already owned
         * property it will calculate the cost of the property (base cost + houses built) and transfer the money to the
         * owner.
         *
         * @param p_turn the player turn.
         * @param player the actual player object.
         * @param dice1 the first dice rolled.
         * @param dice2 the second dice rolled.
         * @param new_pos the new position of the player. Should be passed as described earlier.
         * @param instant determines whether the player will be move instantly or not.
         */
        void move_player(uint8_t p_turn, weak_ptr<Player> player, uint8_t dice1, uint8_t dice2, uint8_t new_pos, bool instant);

        void buy_property(uint8_t property, uint8_t player);

        void buy_property(uint8_t property, string player);

        void buy_house(uint8_t p_turn, uint8_t property, int8_t house_count);

        void player_quit(uint8_t player);

        void remove_player(uint8_t player);

        void player_rejoin(string player);

        GlitchGameContainer get_game_container();

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

        void send_to_all(const json& update);

        const static json not_ok_status;
        const static json ok_status;
    };
}


#endif //ELABORATO_ESAME_GAME_H
