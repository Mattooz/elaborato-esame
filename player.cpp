//
// Created by Niccolò Mattei on 16/07/2020.
//

#include "player.h"

QuarantineGame::Player::Player(string name, double money, uint8_t position, function<void()> quit)
        : name(name), money(money), position(position), quit(quit) {
    this->turns_in_prison = 0;
    this->blocked_for = 0;
    this->avoid = 0;
    this->has_quit = false;
}

const string &QuarantineGame::Player::_name() const {
    return Player::name;
}

double &QuarantineGame::Player::_money() {
    return Player::money;
}

uint8_t &QuarantineGame::Player::_position() {
    return Player::position;
}

uint8_t &QuarantineGame::Player::_turns_in_prison() {
    return Player::turns_in_prison;
}

int8_t &QuarantineGame::Player::_blocked_for() {
    return Player::blocked_for;
}

int8_t &QuarantineGame::Player::_avoid() {
    return Player::avoid;
}

bool QuarantineGame::Player::_has_quit() const {
    return Player::has_quit;
}

void QuarantineGame::Player::add_update(const json &update) {
    if (!has_quit)
        Player::updates.push_back(update);
}

void QuarantineGame::Player::delete_updates() {
    Player::updates.clear();
}

json QuarantineGame::Player::get_update() {
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

bool QuarantineGame::Player::operator==(QuarantineGame::Player p) const {
    return name == p._name() && position != p._position();
}

bool QuarantineGame::Player::operator!=(QuarantineGame::Player p) const {
    return name != p._name() && position != p._position();
}
