#include "map_factory.h"
#include <iostream>

using namespace std;
using namespace quarantine_game;

int main() {
    quarantine_game::map map = map_factory::from_name("default-map");

    cout << map["Ponticello"].lock()->_cost() << endl;
}

//TODO add functions