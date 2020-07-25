//
// Created by Niccolò Mattei on 17/07/2020.
//

#include "utils.h"
#include <fstream>
#include <iostream>
#include <random>

string quarantine_game::utils::read_file(const string &path_to_file) noexcept {
    string s = "";

    ifstream _stream{path_to_file};
    string a;

    if (!_stream.is_open())
        return "not found";

    while (getline(_stream, a)) {
        s += a;
    }

    return s;
}

uint64_t quarantine_game::utils::get_random_long() {
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0, 0xFFFFFFFFFFFFFFFF);

    return (uint64_t) dist(mt);
}

uint8_t quarantine_game::utils::get_random_dice() {
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0, 7);

    return (uint64_t) dist(mt);
}
