//
// Created by Niccolò Mattei on 17/07/2020.
//

#include <iostream>
#include <utility>
#include <utility>
#include "map.h"

//TODO Add const not found

QuarantineGame::FunctionalBox::BoxType QuarantineGame::FunctionalBox::_type() {
    return type;
}

const uint8_t &QuarantineGame::FunctionalBox::_position() const {
    return position;
}

QuarantineGame::FunctionalBox::FunctionalBox(uint8_t position, string type) : Box(position) {

    if(type == "glitch")
        this->type = BoxType::GLITCH;
    else if(type == "prison")
        this->type = BoxType::PRISON;
    else if(type == "goto-prison")
        this->type = BoxType::GOTO_PRISON;
    else if(type == "start")
        this->type = BoxType::START;
    else
        this->type = BoxType::OTHER;

}

QuarantineGame::FunctionalBox::~FunctionalBox() = default;

const uint8_t &QuarantineGame::PropertyBox::_id() const {
    return id;
}

const int32_t &QuarantineGame::PropertyBox::_cost() const {
    return cost;
}

const string &QuarantineGame::PropertyBox::_name() const {
    return name;
}

uint8_t &QuarantineGame::PropertyBox::_owner() {
    return owner;
}

uint8_t &QuarantineGame::PropertyBox::_houses() {
    return houses;
}

const uint8_t &QuarantineGame::PropertyBox::_position() const {
    return position;
}

const uint8_t QuarantineGame::Map::not_found = 0xFF;

QuarantineGame::PropertyBox::PropertyBox(uint8_t position, uint8_t id, int32_t cost, string name) : Box(
        position), id(id), cost(cost), name(std::move(name)) {
    this->owner = Map::not_found;
    this->houses = 0;
}

QuarantineGame::PropertyBox::~PropertyBox() = default;

int32_t QuarantineGame::Map::cost(uint8_t box) const {
    auto a = this->operator[](box);

    if (a) {
        auto b = dynamic_pointer_cast<PropertyBox>(a);
        if (b) return b->_cost();
    }
    return -1;
}

string QuarantineGame::Map::name(uint8_t pos) const {
    auto a = this->operator[](pos);

    if (a) {
        auto b = dynamic_pointer_cast<PropertyBox>(a);
        if (b) return b->_name();
    }
    return "not found";
}

uint8_t QuarantineGame::Map::id(uint8_t position) const {
    if (position > boxes.size()) return Map::not_found;

    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        if ((*it)->_position() != position) continue;

        auto property = dynamic_pointer_cast<PropertyBox>(*it);
        if (property) return property->_id();
    }
    return Map::not_found;
}

uint8_t QuarantineGame::Map::pos(uint8_t id) const {
    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        auto property = dynamic_pointer_cast<PropertyBox>(*it);
        if (property && property->_id() == id) return property->_position();
    }
    return Map::not_found;
}

uint8_t QuarantineGame::Map::distance_to_next_glitch(uint8_t pos) const {
    if (pos >= boxes.size()) return Map::not_found;

    for (int i = 0, l = -1, distance = 1, check = 0;; i = (i + 1) % boxes.size()) {
        shared_ptr<Box> b = boxes[i];

        if (l == -1) {
            if (b->_position() == pos)
                l = i;
        } else {
            if (boxes[i]->_position() == ((boxes[l]->_position() + distance) % boxes.size())) {
                auto functional = dynamic_pointer_cast<FunctionalBox>(b);

                if (functional && functional->_type() == FunctionalBox::BoxType::GLITCH && distance > 1) {
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

bool QuarantineGame::Map::is_glitch(uint8_t pos) const {
    if (id(pos) != Map::not_found) return false;
    else {
        auto a = this->operator[](pos);
        if (a) {
            auto functional = dynamic_pointer_cast<FunctionalBox>(a);

            if (functional && functional->_type() == FunctionalBox::BoxType::GLITCH) return true;
        }
    }
    return false;
}

uint8_t QuarantineGame::Map::distance_to_prison(uint8_t pos) const {
    if (pos >= boxes.size()) return Map::not_found;

    for (int i = 0, l = -1, distance = 1, check = 0;; i = (i + 1) % boxes.size()) {
        shared_ptr<Box> b = boxes[i];

        if (l == -1) {
            if (b->_position() == pos)
                l = i;
        } else {
            if (boxes[i]->_position() == ((boxes[l]->_position() + distance) % boxes.size())) {
                auto functional = dynamic_pointer_cast<FunctionalBox>(b);

                if (functional && functional->_type() == FunctionalBox::BoxType::GOTO_PRISON && distance > 1) {
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

bool QuarantineGame::Map::is_prison(uint8_t pos) const {
    if (id(pos) != Map::not_found) return false;
    else {
        auto a = this->operator[](pos);
        if (a) {
            auto functional = dynamic_pointer_cast<FunctionalBox>(a);

            if (functional && functional->_type() == FunctionalBox::BoxType::GOTO_PRISON) return true;
        }
    }
    return false;
}

vector<shared_ptr<QuarantineGame::PropertyBox>> QuarantineGame::Map::get_player_properties(uint8_t player) const {
    vector<shared_ptr<PropertyBox>> res;

    for (auto &box: boxes) {
        auto property = dynamic_pointer_cast<PropertyBox>(box);
        if (property && property->_owner() == player) res.push_back(shared_ptr<PropertyBox>(property));
    }

    return res;
}

void QuarantineGame::Map::delete_player_properties(uint8_t player) const {
    vector<shared_ptr<PropertyBox>> to_delete = get_player_properties(player);
    for (auto &box: to_delete) {
        if (box) {
            box->_houses() = 0;
            box->_owner() = Map::not_found;
        }
    }
}

shared_ptr<QuarantineGame::Box> QuarantineGame::Map::operator[](uint8_t pos) const {
    if (pos >= boxes.size()) return {};

    for (auto &boxe : boxes) {
        if (boxe->_position() != pos) continue;
        return boxe;
    }
    return {};
}

shared_ptr<QuarantineGame::PropertyBox> QuarantineGame::Map::operator[](const string& name) const {
    for (auto &boxe : boxes) {
        auto property = dynamic_pointer_cast<PropertyBox>(boxe);
        if (property) {
            if (property->_name() != name) continue;
            else return property;
        } else continue;
    }
    return {};
}

shared_ptr<QuarantineGame::PropertyBox> QuarantineGame::Map::from_id(uint8_t id) const {
    for (auto &boxe : boxes) {
        auto property = dynamic_pointer_cast<PropertyBox>(boxe);
        if (property) {
            if (property->_id() != id) continue;
            else return property;
        } else continue;
    }
    return {};
}

QuarantineGame::Map::~Map() {
    for (auto it : boxes) {
        it.reset();
    }
    boxes.clear();
}

QuarantineGame::Map::Map(vector<shared_ptr<Box>> boxes, string map_name, string map_id) : boxes(std::move(boxes)),
                                                                                          map_name(std::move(map_name)),
                                                                                          map_id(std::move(map_id)) {}

const string &QuarantineGame::Map::_map_id() const {
    return map_id;
}

const string &QuarantineGame::Map::_map_name() const {
    return map_name;
}

const uint32_t QuarantineGame::Map::box_count() const {
    return boxes.size();
}

void QuarantineGame::Map::reset() {
    for(auto &boxe : boxes) {
        auto property = dynamic_pointer_cast<PropertyBox>(boxe);
        if(property) {
            property->_houses() = 0;
            property->_owner() = Map::not_found;
        }
    }
}


QuarantineGame::Box::Box(uint8_t position) : position(position) {}

QuarantineGame::Box::~Box() {}

const uint8_t &QuarantineGame::Box::_position() const {
    return position;
}
