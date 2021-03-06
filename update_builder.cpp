#include "update_builder.h"
#include "utils.h"

QuarantineGame::GameUpdateBuilder *QuarantineGame::GameUpdateBuilder::start() {
    builder["isNew"] = true;
    builder["playerCount"] = container.players.size();

    json player_list;

    for (int i = 0; i < container.players.size(); i++) {
        auto pl = container.players[i];
        if(!pl) throw game_error("Error while creating update! Player pointer is expired!");
        json player_details;

        player_details["name"] = pl->_name();
        player_details["money"] = pl->_money();

        auto prp = container.game_map->get_player_properties(i);

        player_details["properties"] = prp.size();

        json properties = json::array();

        for (auto &it : prp) {
            if (!it)
                throw QuarantineGame::game_error(
                        "Error while creating 'property_list' field. Box property expired.");

            properties.push_back(it->_name());
        }

        player_details["property_list"] = properties;

        player_list[to_string(i)] = player_details;
    }

    builder["playerList"] = player_list;
    builder["isTurn"] = *container.turns % container.players.size();

    return this;
}

QuarantineGame::GameUpdateBuilder *
QuarantineGame::GameUpdateBuilder::move(uint8_t dice1, uint8_t dice2, int8_t new_pos, uint8_t player,
                                        bool instant) {
    json update_player_pos;

    update_player_pos["dice1"] = dice1;
    update_player_pos["dice2"] = dice2;
    update_player_pos["playerPos"] = new_pos;
    update_player_pos["playerNum"] = player;
    update_player_pos["instant"] = instant;

    if (!builder.contains("updatePlayerPos")) {
        json array = json::array();

        array.push_back(update_player_pos);
        builder["updatePlayerPos"] = array;
    } else {
        builder["updatePlayerPos"].push_back(update_player_pos);
    }

    return this;
}

QuarantineGame::GameUpdateBuilder *QuarantineGame::GameUpdateBuilder::color(uint8_t property, uint8_t player) {
    json update_property_color;

    update_property_color["pos"] = property;
    update_property_color["player"] = player;

    if (!builder.contains("updatePropertyColor")) {
        json array = json::array();

        array.push_back(update_property_color);
        builder["updatePropertyColor"] = array;
    } else {
        builder["updatePropertyColor"].push_back(update_property_color);
    }

    return this;
}

QuarantineGame::GameUpdateBuilder *
QuarantineGame::GameUpdateBuilder::house_count(uint8_t property, uint8_t house_count) {
    json update_house_count;

    update_house_count["pos"] = property;
    update_house_count["newCount"] = house_count;

    if (!builder.contains("updateHouseCount")) {
        json array = json::array();

        array.push_back(update_house_count);
        builder["updateHouseCount"] = array;
    } else {
        builder["updateHouseCount"].push_back(update_house_count);
    }
    return this;
}

QuarantineGame::GameUpdateBuilder *
QuarantineGame::GameUpdateBuilder::new_glitch(string message, string title, vector<string> buttons) {
    json new_glitch_update;

    new_glitch_update["message"] = message;
    new_glitch_update["title"] = title;

    for (int i = 0; i < buttons.size(); i++)
        new_glitch_update["button" + to_string(i)] = buttons[i];

    new_glitch_update["buttonNum"] = buttons.size();
    new_glitch_update["lockMainButtons"] = true;

    builder["newGlitch"] = new_glitch_update;

    return this;
}

QuarantineGame::GameUpdateBuilder *
QuarantineGame::GameUpdateBuilder::quit(uint8_t player_quit, uint8_t player_to) {
    json player_quit_up;

    if (*container.has_started) {
        player_quit_up["playerId"] = player_quit;
        player_quit_up["newTurn"] = -1;
    } else {
        player_quit_up["playerId"] = player_quit;
        player_quit_up["newTurn"] = player_to;
    }

    builder["playerQuit"] = player_quit_up;

    return this;
}

QuarantineGame::GameUpdateBuilder *QuarantineGame::GameUpdateBuilder::other(string key, string value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GameUpdateBuilder *QuarantineGame::GameUpdateBuilder::other(string key, bool value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GameUpdateBuilder *QuarantineGame::GameUpdateBuilder::other(string key, uint64_t value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GameUpdateBuilder *QuarantineGame::GameUpdateBuilder::other(string key, int64_t value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GameUpdateBuilder *QuarantineGame::GameUpdateBuilder::other(string key, double value) {
    builder[key] = value;
    return this;
}

QuarantineGame::UpdateBuilder *QuarantineGame::GameUpdateBuilder::other_null(string key) {
    builder[key] = nullptr;
    return this;
}

QuarantineGame::GlitchUpdateBuilder *
QuarantineGame::GlitchUpdateBuilder::glitch_error(string message, int32_t glitch) {
    json error;

    error["glitch"] = glitch;
    error["message"] = message;

    if (!builder.contains("errors")) {
        json array = json::array();

        array.push_back(error);
        builder["errors"] = array;
    } else {
        builder["errors"].push_back(error);
    }

    return this;
}

QuarantineGame::GlitchUpdateBuilder *
QuarantineGame::GlitchUpdateBuilder::glitch_warning(string message, int32_t glitch) {
    json warning;

    warning["glitch"] = glitch;
    warning["message"] = message;

    if (!builder.contains("warnings")) {
        json array = json::array();

        array.push_back(warning);
        builder["warnings"] = array;
    } else {
        builder["warnings"].push_back(warning);
    }

    return this;
}

QuarantineGame::GlitchUpdateBuilder *
QuarantineGame::GlitchUpdateBuilder::glitch_success(string message, int32_t glitch) {
    json success;

    success["glitch"] = glitch;
    success["message"] = message;

    if (!builder.contains("successes")) {
        json array = json::array();

        array.push_back(success);
        builder["successes"] = array;
    } else {
        builder["successes"].push_back(success);
    }

    return this;
}

QuarantineGame::GlitchUpdateBuilder *QuarantineGame::GlitchUpdateBuilder::other(string key, string value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GlitchUpdateBuilder *QuarantineGame::GlitchUpdateBuilder::other(string key, bool value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GlitchUpdateBuilder *QuarantineGame::GlitchUpdateBuilder::other(string key, uint64_t value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GlitchUpdateBuilder *QuarantineGame::GlitchUpdateBuilder::other(string key, int64_t value) {
    builder[key] = value;
    return this;
}

QuarantineGame::GlitchUpdateBuilder *QuarantineGame::GlitchUpdateBuilder::other(string key, double value) {
    builder[key] = value;
    return this;
}

QuarantineGame::UpdateBuilder *QuarantineGame::GlitchUpdateBuilder::other_null(string key) {
    builder[key] = nullptr;
    return this;
}
