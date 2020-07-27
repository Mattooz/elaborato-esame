//
// Created by Niccolò Mattei on 17/07/2020.
//

#include "utils.h"
#include <fstream>
#include <iostream>
#include <random>
#include <regex>

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
    mt19937_64 mt(rd());
    uniform_int_distribution<uint64_t > dist(numeric_limits<uint64_t >::min(),
                                                       numeric_limits<uint64_t >::max());

    return (uint64_t) dist(mt);
}

uint8_t quarantine_game::utils::get_random_dice() {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<uint8_t> dist(0, 7);

    return (uint8_t) dist(mt);
}

uint32_t quarantine_game::utils::get_random_num(uint32_t max) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<uint32_t> dist(0, max);

    return (uint32_t) dist(mt);
}

std::vector<std::string> quarantine_game::utils::split(const string& input, const string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
            last;
    return {first, last};
}

