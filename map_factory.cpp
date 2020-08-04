//
// Created by Niccolò Mattei on 21/07/2020.
//

#include "map_factory.h"
#include "utils.h"
#include <filesystem>
#include <nlohmann/json.hpp>

using namespace std::filesystem;
using json = nlohmann::json;

const string quarantine_game::MapFactory::map_folder = "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/maps";
vector<quarantine_game::Map> quarantine_game::MapFactory::cached = {};

quarantine_game::Map quarantine_game::MapFactory::from_name(string name) {
    if (cached.empty()) map_list();
    for (auto &it : cached) {
        if (it._map_name() == name) return it;
    }
    return quarantine_game::Map{{}, "not found", "not found"};
}

quarantine_game::Map quarantine_game::MapFactory::from_id(string id) {
    if (cached.empty()) map_list();
    for (auto &it : cached)
        if (it._map_id() == id) return it;
    return quarantine_game::Map{{}, "not found", "not found"};
}

vector<quarantine_game::Map> quarantine_game::MapFactory::map_list() {
    if (cached.empty()) {
        for (const auto &it : directory_iterator(MapFactory::map_folder)) {
            vector<shared_ptr<Box>> vec;
            string s = Utils::read_file(it.path());
            json parsed = json::parse(s);
            string name = parsed["name"];
            string id = parsed["id"];

            json boxes = parsed["boxes"];

            for (auto &it1 : boxes) {
                shared_ptr<Box> to_add;

                if (it1.contains("id")) {
                    to_add = make_shared<PropertyBox>(
                            PropertyBox(it1["position"], it1["id"], it1["cost"], it1["name"]));
                } else {
                    to_add = make_shared<FunctionalBox>(FunctionalBox(it1["position"], it1["type"]));
                }

                vec.push_back(to_add);
            }

            quarantine_game::Map map{vec, name, id};

            cached.push_back(map);
            return cached;
        }
    } else return cached;
}
