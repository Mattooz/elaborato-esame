#include <nlohmann/json.hpp>
#include <string>

#include "player.h"
#include "map.h"

#ifndef ELABORATO_ESAME_UPDATE_BUILDER_H
#define ELABORATO_ESAME_UPDATE_BUILDER_H

using namespace std;
using namespace nlohmann;


namespace quarantine_game {

    /**
     * Contains all the info necessary to the game_update_builder class.
     * All members are public and read-only.
     */
    struct UpdateGameContainer {
        /*
         * The members of this struct are vectors pointing to the objects inside the Game class.
         */
        const vector<weak_ptr<Player>> players;
        const uint32_t *turns;
        const bool *has_started;
        Map *game_map;

        UpdateGameContainer(const vector<weak_ptr<Player>> players, const uint32_t *turns, const bool *hasStarted,
                            Map *gameMap) : players(players), turns(turns), has_started(hasStarted),
                                              game_map(gameMap) {}
    };

    class UpdateBuilder {
    public:
        /**
         * "Other" methods add a given key and value to the json object.
         * Using overloaded "other" methods because of lack of generic Object superclass (such as the one in Java).
         * Ugly but necessary.
         *
         * @return all return the builder.
         */
        virtual UpdateBuilder *other(string key, string value) = 0;

        virtual UpdateBuilder *other(string key, bool value) = 0;

        virtual UpdateBuilder *other(string key, uint64_t value) = 0;

        virtual UpdateBuilder *other(string key, int64_t value) = 0;

        virtual UpdateBuilder *other(string key, double value) = 0;

        virtual UpdateBuilder *other_null(string key) = 0;

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
    class GameUpdateBuilder : public UpdateBuilder {
    private:
        /*
         * Contains all of the info necessary to the builder
         */
        UpdateGameContainer container;
        json builder;

    public:

        GameUpdateBuilder() = delete;

        /**
         * Creates a brand new builder.
         *
         * @param container the game container needed by the builder
         */
        explicit GameUpdateBuilder(UpdateGameContainer container) : container(container) {}

        /**
         * Modifies an already existing json update.
         *
         * @param container the game container needed by the builder
         * @param builder the existing json update
         */
        GameUpdateBuilder(UpdateGameContainer container, json builder) : container(container),
                                                                         builder(builder) {}

        /**
         * Adds the required information for a game update.
         * All updates should use with this method.
         *
         * @return the builder
         */
        GameUpdateBuilder *start();

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
        GameUpdateBuilder *move(uint8_t dice1, uint8_t dice2, int8_t new_pos, uint8_t player, bool instant);

        /**
         * Changes the color of the property chosen based on the id of the given player.
         *
         * @param property the id of the property on the board.
         * @param player the id of the player.
         * @return the builder
         */
        GameUpdateBuilder *color(uint8_t property, uint8_t player);

        /**
         * Changes the house count on a certain property.
         *
         * @param property the id of the property on the board
         * @param house_count the number of houses. 0 is no houses while 5 is an hotel
         * @return the builder
         */
        GameUpdateBuilder *house_count(uint8_t property, uint8_t house_count);

        /**
         * Adds a glitch to the update.
         *
         * @param message the message of the glitch
         * @param title the title of the glitch
         * @param buttons the buttons of the glitch
         * @return the builder
         */
        GameUpdateBuilder *new_glitch(string message, string title, vector<string> buttons);

        /**
         * Adds a quit update to the json object.
         *
         * @param player_quit the player that quit
         * @param player_to if the game hasn't started this should be the new turn of the player
         */
        GameUpdateBuilder *quit(uint8_t player_quit, uint8_t player_to);

        /*
         * Overridden methods.
         */
        GameUpdateBuilder *other(string key, string value) override;

        GameUpdateBuilder *other(string key, bool value) override;

        GameUpdateBuilder *other(string key, uint64_t value) override;

        GameUpdateBuilder *other(string key, int64_t value) override;

        GameUpdateBuilder *other(string key, double value) override;

        UpdateBuilder *other_null(string key) override;

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
    class GlitchUpdateBuilder : public UpdateBuilder {
    private:
        json builder;
    public:
        /**
        * Creates a brand new builder.
        */
        GlitchUpdateBuilder() = default;

        /**
         * Modifies an already existing json update.
         *
         * @param builder the existing json update
         */
        explicit GlitchUpdateBuilder(json builder) : builder(builder) {}


        /**
         * Adds a glitch error message to the json update. This will be displayed with red colored text on the builder.
         * Errors prevent the glitch list from working correctly and must be fixed.
         *
         * @param message the message of the error
         * @param glitch the id of the glitch
         * @return the builder
         */
        GlitchUpdateBuilder *glitch_error(string message, int32_t glitch);

        /**
         * Adds a glitch warning message to the json update. This will be displayed with orange colored text on the
         * builder.
         * Warnings do not prevent the list from working, but should be still fixed.
         *
         * @param message the message of the warning
         * @param glitch the id of the glitch
         * @return the builder
         */
        GlitchUpdateBuilder *glitch_warning(string message, int32_t glitch);

        /**
         * Adds a glitch success message to the json update. This will be displayed with green colored text on the
         * builder.
         * Indicates a successful action made by the user.
         *
         * @param message the message
         * @param glitch the id of the glitch
         * @return the builder
         */
        GlitchUpdateBuilder *glitch_success(string message, int32_t glitch);

        /*
         * Overridden methods.
         */
        GlitchUpdateBuilder *other(string key, string value) override;

        GlitchUpdateBuilder *other(string key, bool value) override;

        GlitchUpdateBuilder *other(string key, uint64_t value) override;

        GlitchUpdateBuilder *other(string key, int64_t value) override;

        GlitchUpdateBuilder *other(string key, double value) override;

        UpdateBuilder *other_null(string key) override;

        json res() override {
            return builder;
        }
    };

}

#endif //ELABORATO_ESAME_UPDATE_BUILDER_H
