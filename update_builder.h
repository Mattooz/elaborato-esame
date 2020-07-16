#include <nlohmann/json.hpp>
#include <string>

#include "player.h"

#ifndef ELABORATO_ESAME_UPDATE_BUILDER_H
#define ELABORATO_ESAME_UPDATE_BUILDER_H

using namespace std;
using namespace nlohmann;


namespace quarantine_game {

    /**
     * Contains all the info necessary to the game_update_builder class.
     * All members are public and read-only.
     */
    struct update_game_container {
        const vector<player> *players;
        const uint32_t *turns;
        const bool *has_started;

        update_game_container(const vector<player> *players, const uint32_t *turns, const bool *hasStarted) : players(
                players), turns(turns), has_started(hasStarted) {}
    };

    class update_builder {
    public:
        /**
         * "Other" methods add a given key and value.
         * Using overloaded "other" methods because of lack of generic Object superclass (such as the one in Java).
         * Ugly but necessary.
         *
         * @return all return the builder.
         */
        virtual update_builder *other(string key, string value) = 0;

        virtual update_builder *other(string key, bool value) = 0;

        virtual update_builder *other(string key, uint64_t value) = 0;

        virtual update_builder *other(string key, int64_t value) = 0;

        virtual update_builder *other(string key, double value) = 0;

        /**
        * Returns the finished json object
        * @return a json object
        */
        virtual json res() = 0;
    };

    /**
     * Update builder class for the Game class. Used to build json updates for the game client.
     */
    class game_update_builder : public update_builder {
    private:
        /*
         * Contains all of the info necessary to the builder
         */
        update_game_container container;
        json builder;

    public:
        /**
         * Adds the required information for a game update.
         * All updates should use with this method.
         *
         * @return the builder
         */
        game_update_builder *start();

        /**
         * Adds a movement update to the current update.
         *
         * @param dice1 the number shown by the client in the first dice box
         * @param dice2 the number shown by the client in the second dice box
         * @param new_pos if the movement is instant this should be the new position the player on the map. Otherwise it
         *                should the sum of the two dice rolled.
         * @param player the number of the player that should be moved.
         * @param instant determines whether the player will be move instantly or not.
         *
         * @return the builder
         */
        game_update_builder *move(uint8_t dice1, uint8_t dice2, uint8_t new_pos, uint8_t player, bool instant);

        /**
         * Changes the color of the property chosen based on the id of the given player.
         *
         * @param property the id of the property on the board.
         * @param player the id of the player.
         * @return
         */
        game_update_builder *color(uint8_t property, uint8_t player);

        /**
         * Changes the house count on a certain property.
         *
         * @param property the id of the property on the board
         * @param house_count the number of houses. 0 is no houses while 5 is an hotel
         * @return the builder
         */
        game_update_builder *house_count(uint8_t property, uint8_t house_count);

        /**
         * Adds a glitch to the update.
         *
         * @param message the message of the glitch
         * @param title the title of the glitch
         * @param buttons the buttons of the glitch
         * @return the builder
         */
        game_update_builder *new_glitch(string message, string title, vector<string> buttons);

        /**
         * Adds a quit update to the json update.
         *
         * @param player_quit the player that quit
         * @param player_to if the game hasn't started this should be the new turn of the player
         */
        game_update_builder *quit(uint8_t player_quit, uint8_t player_to);

        //TODO add the definitions
        game_update_builder *other(string key, string value) override;

        game_update_builder *other(string key, bool value) override;

        game_update_builder *other(string key, uint64_t value) override;

        game_update_builder *other(string key, int64_t value) override;

        game_update_builder *other(string key, double value) override;

        json res() override {
            return builder;
        }
    };

}

#endif //ELABORATO_ESAME_UPDATE_BUILDER_H
