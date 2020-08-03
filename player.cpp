//
// Created by Niccolò Mattei on 16/07/2020.
//

#include "player.h"

quarantine_game::Player::Player(string name, double money, uint8_t position, function<void()> quit)
        : name(name), money(money), position(position), quit(quit) {
    this->turns_in_prison = 0;
    this->blocked_for = 0;
    this->avoid = 0;
    this->has_quit = false;
}

const string &quarantine_game::Player::_name() const {
    return Player::name;
}

double &quarantine_game::Player::_money() {
    return Player::money;
}

uint8_t &quarantine_game::Player::_position() {
    return Player::position;
}

uint8_t &quarantine_game::Player::_turns_in_prison() {
    return Player::turns_in_prison;
}

int8_t &quarantine_game::Player::_blocked_for() {
    return Player::blocked_for;
}

int8_t &quarantine_game::Player::_avoid() {
    return Player::avoid;
}

bool quarantine_game::Player::_has_quit() const {
    return Player::has_quit;
}

void quarantine_game::Player::add_update(const json &update) {
    if (!has_quit)
        Player::updates.push_back(update);
}

void quarantine_game::Player::delete_updates() {
    Player::updates.clear();
}

json quarantine_game::Player::get_update() {
    if (has_quit) {
        has_quit = false;
        //TODO add send rejoin update
    }

    json res;

    if (updates.size() == 0) {
        res["isNew"] = false;
    } else {
        res = updates[0];
        updates.erase(updates.begin());
    }

    return res;
}

bool quarantine_game::Player::operator==(quarantine_game::Player p) {
    return name == p._name() && position != p._position();
}

bool quarantine_game::Player::operator!=(quarantine_game::Player p) {
    return name != p._name() && position != p._position();
}
