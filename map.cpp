//
// Created by Niccolò Mattei on 17/07/2020.
//

#include <iostream>
#include "map.h"


string quarantine_game::functional_box::_type() {
    return type;
}

const uint8_t &quarantine_game::functional_box::_position() const {
    return position;
}

quarantine_game::functional_box::functional_box(uint8_t position, const string &type) : box(position), type(type) {}

quarantine_game::functional_box::~functional_box() {}

const uint8_t &quarantine_game::property_box::_id() const {
    return id;
}

const int32_t &quarantine_game::property_box::_cost() const {
    return cost;
}

const string &quarantine_game::property_box::_name() const {
    return name;
}

uint8_t &quarantine_game::property_box::_owner() {
    return owner;
}

uint8_t &quarantine_game::property_box::_houses() {
    return houses;
}

const uint8_t &quarantine_game::property_box::_position() const {
    return position;
}

quarantine_game::property_box::property_box(uint8_t position, uint8_t id, int32_t cost, const string &name) : box(
        position), id(id), cost(cost), name(name) {}

quarantine_game::property_box::~property_box() {}

quarantine_game::map::map(const vector<box *> &boxes) : boxes(boxes) {}

int32_t quarantine_game::map::cost(uint8_t box) {
    auto a = this->operator[](box);

    if (a) {
        auto b = dynamic_cast<property_box *>(a);
        if (b) return b->_cost();
    }
    return -1;
}

string quarantine_game::map::name(uint8_t pos) {
    auto a = this->operator[](pos);

    if (a) {
        auto b = dynamic_cast<property_box *>(a);
        if (b) return b->_name();
    }
    return "not found";
}

uint8_t quarantine_game::map::id(uint8_t position) {
    if (position > boxes.size()) return 0xFF;

    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        if ((*it)->_position() != position) continue;

        auto property = dynamic_cast<property_box *>(*it);
        if (property) return property->_id();
    }
    return 0xFF;
}

uint8_t quarantine_game::map::pos(uint8_t id) {
    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        auto property = dynamic_cast<property_box *>(*it);
        if (property && property->_id() == id) return property->_position();
    }
    return 0xFF;
}

uint8_t quarantine_game::map::distance_to_next_glitch(uint8_t pos) {
    if (pos >= boxes.size()) return 0xFF;

    for (int i = 0, l = -1, distance = 1, check = 0;; i = (i + 1) % boxes.size()) {
        box *b = boxes[i];

        if (l == -1) {
            if (b->_position() == pos)
                l = i;
        } else {
            if (boxes[i]->_position() == ((boxes[l]->_position() + distance) % boxes.size())) {
                auto functional = dynamic_cast<functional_box *>(b);

                if (functional && functional->_type() == "glitch" && distance > 1) {
                    return distance;
                }

                distance++;
                check++;
            }
        }
        if (check == boxes.size() + 1) break;
    }
    return 0xFF;
}

bool quarantine_game::map::is_glitch(uint8_t pos) {
    if (id(pos) != 0xFF) return false;
    else {
        auto a = this->operator[](pos);
        if (a) {
            auto functional = dynamic_cast<functional_box *>(a);

            if (functional && functional->_type() == "glitch") return true;
        }
    }
    return false;
}

uint8_t quarantine_game::map::distance_to_prison(uint8_t pos) {
    if (pos >= boxes.size()) return 0xFF;

    for (int i = 0, l = -1, distance = 1, check = 0;; i = (i + 1) % boxes.size()) {
        box *b = boxes[i];

        if (l == -1) {
            if (b->_position() == pos)
                l = i;
        } else {
            if (boxes[i]->_position() == ((boxes[l]->_position() + distance) % boxes.size())) {
                auto functional = dynamic_cast<functional_box *>(b);

                if (functional && functional->_type() == "goto-prison" && distance > 1) {
                    return distance;
                }

                distance++;
                check++;
            }
        }
        if (check == boxes.size() + 1) break;
    }
    return 0xFF;
}

bool quarantine_game::map::is_prison(uint8_t pos) {
    if (id(pos) != 0xFF) return false;
    else {
        auto a = this->operator[](pos);
        if (a) {
            auto functional = dynamic_cast<functional_box *>(a);

            if (functional && functional->_type() == "goto-prison") return true;
        }
    }
    return false;
}

vector<quarantine_game::property_box *> quarantine_game::map::get_player_properties(uint8_t player) {
    vector<property_box *> res;

    for (auto &box: boxes) {
        auto property = dynamic_cast<property_box *>(box);
        if (property && property->_owner() == player) res.push_back(property);
    }

    return res;
}

void quarantine_game::map::delete_player_properties(uint8_t player) {
    vector<property_box *> to_delete = get_player_properties(player);
    for (auto &box: to_delete) {
        box->_houses() = 0;
        box->_owner() = 7;
    }
}

quarantine_game::box *quarantine_game::map::operator[](uint8_t pos) {
    if (pos >= boxes.size()) return nullptr;

    for (auto &boxe : boxes) {
        if (boxe->_position() != pos) continue;
        return boxe;
    }
    return nullptr;
}

quarantine_game::property_box *quarantine_game::map::operator[](string name) {
    for (auto &boxe : boxes) {
        auto property = dynamic_cast<property_box *>(boxe);
        if (property) {
            if (property->_name() != name) continue;
            else return property;
        } else continue;
    }
    return nullptr;
}

quarantine_game::property_box *quarantine_game::map::from_id(uint8_t id) {
    for (auto &boxe : boxes) {
        auto property = dynamic_cast<property_box *>(boxe);
        if (property) {
            if (property->_id() != id) continue;
            else return property;
        } else continue;
    }
    return nullptr;
}

quarantine_game::map::~map() {
    for(auto & it : boxes) {
        delete it;
    }
}

quarantine_game::box::box(uint8_t position) : position(position) {}

quarantine_game::box::~box() {}

const uint8_t &quarantine_game::box::_position() const {
    return position;
}
