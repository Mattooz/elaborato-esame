//
// Created by Niccolò Mattei on 17/07/2020.
//

#include <iostream>
#include <utility>
#include <utility>
#include "map.h"

//TODO Add const not found

string quarantine_game::FunctionalBox::_type() {
    return type;
}

const uint8_t &quarantine_game::FunctionalBox::_position() const {
    return position;
}

quarantine_game::FunctionalBox::FunctionalBox(uint8_t position, string type) : Box(position), type(std::move(type)) {}

quarantine_game::FunctionalBox::~FunctionalBox() = default;

const uint8_t &quarantine_game::PropertyBox::_id() const {
    return id;
}

const int32_t &quarantine_game::PropertyBox::_cost() const {
    return cost;
}

const string &quarantine_game::PropertyBox::_name() const {
    return name;
}

uint8_t &quarantine_game::PropertyBox::_owner() {
    return owner;
}

uint8_t &quarantine_game::PropertyBox::_houses() {
    return houses;
}

const uint8_t &quarantine_game::PropertyBox::_position() const {
    return position;
}

quarantine_game::PropertyBox::PropertyBox(uint8_t position, uint8_t id, int32_t cost, string name) : Box(
        position), id(id), cost(cost), name(std::move(name)) {
    this->owner = Map::not_found;
    this->houses = 0;
}

quarantine_game::PropertyBox::~PropertyBox() = default;

int32_t quarantine_game::Map::cost(uint8_t box) {
    auto a = this->operator[](box).lock();

    if (a) {
        auto b = dynamic_pointer_cast<PropertyBox>(a);
        if (b) return b->_cost();
    }
    return -1;
}

string quarantine_game::Map::name(uint8_t pos) {
    auto a = this->operator[](pos).lock();

    if (a) {
        auto b = dynamic_pointer_cast<PropertyBox>(a);
        if (b) return b->_name();
    }
    return "not found";
}

uint8_t quarantine_game::Map::id(uint8_t position) {
    if (position > boxes.size()) return Map::not_found;

    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        if ((*it)->_position() != position) continue;

        auto property = dynamic_pointer_cast<PropertyBox>(*it);
        if (property) return property->_id();
    }
    return Map::not_found;
}

uint8_t quarantine_game::Map::pos(uint8_t id) {
    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        auto property = dynamic_pointer_cast<PropertyBox>(*it);
        if (property && property->_id() == id) return property->_position();
    }
    return Map::not_found;
}

uint8_t quarantine_game::Map::distance_to_next_glitch(uint8_t pos) {
    if (pos >= boxes.size()) return Map::not_found;

    for (int i = 0, l = -1, distance = 1, check = 0;; i = (i + 1) % boxes.size()) {
        shared_ptr<Box> b = boxes[i];

        if (l == -1) {
            if (b->_position() == pos)
                l = i;
        } else {
            if (boxes[i]->_position() == ((boxes[l]->_position() + distance) % boxes.size())) {
                auto functional = dynamic_pointer_cast<FunctionalBox>(b);

                if (functional && functional->_type() == "glitch" && distance > 1) {
                    return distance;
                }

                distance++;
                check++;
            }
        }
        if (check == boxes.size() + 1) break;
    }
    return Map::not_found;
}

bool quarantine_game::Map::is_glitch(uint8_t pos) {
    if (id(pos) != Map::not_found) return false;
    else {
        auto a = this->operator[](pos).lock();
        if (a) {
            auto functional = dynamic_pointer_cast<FunctionalBox>(a);

            if (functional && functional->_type() == "glitch") return true;
        }
    }
    return false;
}

uint8_t quarantine_game::Map::distance_to_prison(uint8_t pos) {
    if (pos >= boxes.size()) return Map::not_found;

    for (int i = 0, l = -1, distance = 1, check = 0;; i = (i + 1) % boxes.size()) {
        shared_ptr<Box> b = boxes[i];

        if (l == -1) {
            if (b->_position() == pos)
                l = i;
        } else {
            if (boxes[i]->_position() == ((boxes[l]->_position() + distance) % boxes.size())) {
                auto functional = dynamic_pointer_cast<FunctionalBox>(b);

                if (functional && functional->_type() == "goto-prison" && distance > 1) {
                    return distance;
                }

                distance++;
                check++;
            }
        }
        if (check == boxes.size() + 1) break;
    }
    return Map::not_found;
}

bool quarantine_game::Map::is_prison(uint8_t pos) {
    if (id(pos) != Map::not_found) return false;
    else {
        auto a = this->operator[](pos).lock();
        if (a) {
            auto functional = dynamic_pointer_cast<FunctionalBox>(a);

            if (functional && functional->_type() == "goto-prison") return true;
        }
    }
    return false;
}

vector<weak_ptr<quarantine_game::PropertyBox>> quarantine_game::Map::get_player_properties(uint8_t player) {
    vector<weak_ptr<PropertyBox>> res;

    for (auto &box: boxes) {
        auto property = dynamic_pointer_cast<PropertyBox>(box);
        if (property && property->_owner() == player) res.push_back(weak_ptr<PropertyBox>(property));
    }

    return res;
}

void quarantine_game::Map::delete_player_properties(uint8_t player) {
    vector<weak_ptr<PropertyBox>> to_delete = get_player_properties(player);
    for (auto &box: to_delete) {
        if (auto p = box.lock()) {
            p->_houses() = 0;
            p->_owner() = Map::not_found;
        }
    }
}

weak_ptr<quarantine_game::Box> quarantine_game::Map::operator[](uint8_t pos) {
    if (pos >= boxes.size()) return {};

    for (auto &boxe : boxes) {
        if (boxe->_position() != pos) continue;
        return boxe;
    }
    return {};
}

weak_ptr<quarantine_game::PropertyBox> quarantine_game::Map::operator[](const string& name) {
    for (auto &boxe : boxes) {
        auto property = dynamic_pointer_cast<PropertyBox>(boxe);
        if (property) {
            if (property->_name() != name) continue;
            else return property;
        } else continue;
    }
    return {};
}

weak_ptr<quarantine_game::PropertyBox> quarantine_game::Map::from_id(uint8_t id) {
    for (auto &boxe : boxes) {
        auto property = dynamic_pointer_cast<PropertyBox>(boxe);
        if (property) {
            if (property->_id() != id) continue;
            else return property;
        } else continue;
    }
    return {};
}

quarantine_game::Map::~Map() {
    for (auto it : boxes) {
        it.reset();
    }
    boxes.clear();
}

quarantine_game::Map::Map(vector<shared_ptr<Box>> boxes, string map_name, string map_id) : boxes(std::move(boxes)),
                                                                                           map_name(std::move(map_name)),
                                                                                           map_id(std::move(map_id)) {}

const string &quarantine_game::Map::_map_id() const {
    return map_id;
}

const string &quarantine_game::Map::_map_name() const {
    return map_name;
}

const uint32_t quarantine_game::Map::box_count() const {
    return boxes.size();
}


quarantine_game::Box::Box(uint8_t position) : position(position) {}

quarantine_game::Box::~Box() {}

const uint8_t &quarantine_game::Box::_position() const {
    return position;
}
