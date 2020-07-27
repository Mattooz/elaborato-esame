//
// Created by Niccolò Mattei on 16/07/2020.
//

#include "player.h"

quarantine_game::player::player(string name, double money, uint8_t position, function<void()> quit)
        : name(name), money(money), position(position), quit(quit) {
    this->turns_in_prison = 0;
    this->blocked_for = 0;
    this->avoid = 0;
    this->has_quit = false;
}

const string &quarantine_game::player::_name() const {
    return player::name;
}

double &quarantine_game::player::_money() {
    return player::money;
}

uint8_t & quarantine_game::player::_position() {
    return player::position;
}

uint8_t &quarantine_game::player::_turns_in_prison() {
    return player::turns_in_prison;
}

int8_t &quarantine_game::player::_blocked_for() {
    return player::blocked_for;
}

int8_t &quarantine_game::player::_avoid() {
    return player::avoid;
}

bool quarantine_game::player::_has_quit() const {
    return player::has_quit;
}

void quarantine_game::player::add_update(const json &update) {
    if (!has_quit)
        player::updates.push_back(update);
}

void quarantine_game::player::delete_updates() {
    player::updates.clear();
}

json quarantine_game::player::get_update() {
    if (player::has_quit) {
        player::has_quit = false;
        //TODO add send rejoin update
    }

    if (updates.empty()) {
        json res;

        res["isNew"] = false;

        return res;
    } else {
        json res = updates[0];
        updates.erase(updates.begin());
    }
}

bool quarantine_game::player::operator==(quarantine_game::player p) {
    return name == p._name() && position != p._position();
}

bool quarantine_game::player::operator!=(quarantine_game::player p) {
    return name != p._name() && position != p._position();
}
