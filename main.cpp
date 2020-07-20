#include <iostream>
#include <nlohmann/json.hpp>
#include "utils.h"

using namespace std;
using namespace quarantine_game;
using json = nlohmann::json;

int main() {
    cout << utils::read_file(
            "/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/game_builder_json_output.json") << endl;
}

//TODO add functions