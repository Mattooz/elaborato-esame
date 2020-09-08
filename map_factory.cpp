//
// Created by Niccolò Mattei on 21/07/2020.
//

#include "map_factory.h"
#include "utils.h"
#include <filesystem>
#include <nlohmann/json.hpp>

using namespace std::filesystem;
using json = nlohmann::json;

//TODO "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/maps";

const QuarantineGame::Map QuarantineGame::MapFactory::not_found = {{}, "not found", "not found"};

QuarantineGame::Map QuarantineGame::MapFactory::from_name(string name) {
    for (auto &it : map_list()) {
        if (it._map_name() == name) return it;
    }
    return QuarantineGame::MapFactory::not_found;
}

QuarantineGame::Map QuarantineGame::MapFactory::from_id(string id) {
    for (auto &it : map_list())
        if (it._map_id() == id) return it;
    return QuarantineGame::MapFactory::not_found;
}

vector<QuarantineGame::Map> QuarantineGame::MapFactory::map_list() {
    vector<QuarantineGame::Map> res;

    string map_folder = "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/maps";

    for (const auto &it : directory_iterator(map_folder)) {
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

        QuarantineGame::Map map{vec, name, id};

        res.push_back(map);
    }
    return res;
}