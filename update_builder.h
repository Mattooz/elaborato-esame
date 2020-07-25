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
        /*
         * The members of this struct are vectors pointing to the objects inside the game class.
         */
        const vector<player> *players;
        const uint32_t *turns;
        const bool *has_started;

        update_game_container(const vector<player> *players, const uint32_t *turns, const bool *hasStarted) : players(
                players), turns(turns), has_started(hasStarted) {}
    };

    class update_builder {
    public:
        /**
         * "Other" methods add a given key and value to the json object.
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

        virtual update_builder *other_null(string key) = 0;

        /**
        * Returns the finished json object
        * @return a json object
        */
        virtual json res() = 0;
    };

    /**
     * Update builder class for the Game class @see quarantine_game::game.
     * Used to build json updates for the game client.
     */
    class game_update_builder : public update_builder {
    private:
        /*
         * Contains all of the info necessary to the builder
         */
        update_game_container container;
        json builder;

    public:

        game_update_builder() = delete;

        /**
         * Creates a brand new builder.
         *
         * @param container the game container needed by the builder
         */
        explicit game_update_builder(update_game_container container) : container(container) {}

        /**
         * Modifies an already existing json update.
         *
         * @param container the game container needed by the builder
         * @param builder the existing json update
         */
        game_update_builder(update_game_container container, json builder) : container(container),
                                                                             builder(builder) {}

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
         * @param new_pos if the movement is instant this should be the new position of the player on the map. Otherwise
         *        it should the sum of the two dice rolled.
         * @param player the id of the player that should be moved.
         * @param instant determines whether the player will be move instantly or not.
         *
         * @return the builder
         */
        game_update_builder *move(uint8_t dice1, uint8_t dice2, int8_t new_pos, uint8_t player, bool instant);

        /**
         * Changes the color of the property chosen based on the id of the given player.
         *
         * @param property the id of the property on the board.
         * @param player the id of the player.
         * @return the builder
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
         * Adds a quit update to the json object.
         *
         * @param player_quit the player that quit
         * @param player_to if the game hasn't started this should be the new turn of the player
         */
        game_update_builder *quit(uint8_t player_quit, uint8_t player_to);

        /*
         * Overridden methods.
         */
        game_update_builder *other(string key, string value) override;

        game_update_builder *other(string key, bool value) override;

        game_update_builder *other(string key, uint64_t value) override;

        game_update_builder *other(string key, int64_t value) override;

        game_update_builder *other(string key, double value) override;

        update_builder *other_null(string key) override;

        json res() override {
            return builder;
        }
    };

    /**
     * Update builder used by the glitch_handler class @see quarantine_game::glitch_handler.
     * Used to build the updates sent to the Glitch Builder web interface.
     *
     * The glitch id can also be -1 and -2: -1 indicates general list errors such as "missing name", -2 indicates server
     * errors.
     */
    class glitch_update_builder : public update_builder {
    private:
        json builder;
    public:
        /**
        * Creates a brand new builder.
        */
        glitch_update_builder() = default;

        /**
         * Modifies an already existing json update.
         *
         * @param builder the existing json update
         */
        explicit glitch_update_builder(json builder) : builder(builder) {}


        /**
         * Adds a glitch error message to the json update. This will be displayed with red colored text on the builder.
         * Errors prevent the glitch list from working correctly and must be fixed.
         *
         * @param message the message of the error
         * @param glitch the id of the glitch
         * @return the builder
         */
        glitch_update_builder *glitch_error(string message, int32_t glitch);

        /**
         * Adds a glitch warning message to the json update. This will be displayed with orange colored text on the
         * builder.
         * Warnings do not prevent the list from working, but should be still fixed.
         *
         * @param message the message of the warning
         * @param glitch the id of the glitch
         * @return the builder
         */
        glitch_update_builder *glitch_warning(string message, int32_t glitch);

        /**
         * Adds a glitch success message to the json update. This will be displayed with green colored text on the
         * builder.
         * Indicates a successful action made by the user.
         *
         * @param message the message
         * @param glitch the id of the glitch
         * @return the builder
         */
        glitch_update_builder *glitch_success(string message, int32_t glitch);

        /*
         * Overridden methods.
         */
        glitch_update_builder *other(string key, string value) override;

        glitch_update_builder *other(string key, bool value) override;

        glitch_update_builder *other(string key, uint64_t value) override;

        glitch_update_builder *other(string key, int64_t value) override;

        glitch_update_builder *other(string key, double value) override;

        update_builder *other_null(string key) override;

        json res() override {
            return builder;
        }
    };

}

#endif //ELABORATO_ESAME_UPDATE_BUILDER_H
