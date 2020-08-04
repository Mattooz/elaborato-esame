//
// Created by Niccolò Mattei on 17/07/2020.
//

#include "utils.h"
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <boost/locale.hpp>

using namespace boost::locale;

string quarantine_game::Utils::read_file(const string &path_to_file) noexcept {
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

wstring quarantine_game::Utils::read_utf8_file(const string &path_to_file) noexcept {
    wstring begin = conv::to_utf<wchar_t>(read_file(path_to_file), "UTF-8");
    wstring end = conv::utf_to_utf<wchar_t>(begin);

    return end;
}

uint64_t quarantine_game::Utils::get_random_long() {
    random_device rd;
    mt19937_64 mt(rd());
    uniform_int_distribution<uint64_t> dist(numeric_limits<uint64_t>::min(),
                                            numeric_limits<uint64_t>::max());

    return (uint64_t) dist(mt);
}

uint8_t quarantine_game::Utils::get_random_dice() {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<uint8_t> dist(1, 6);

    //return (uint8_t) dist(mt);
    return 4;
}

uint32_t quarantine_game::Utils::get_random_num(uint32_t max) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<uint32_t> dist(0, max);

    return (uint32_t) dist(mt);
}

std::vector<std::string> quarantine_game::Utils::split(const string &input, const string &regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
            last;
    return {first, last};
}

bool quarantine_game::Utils::is_number(string s) {
    try {
        stod(s);
        return true;
    } catch (exception &e) {
        return false;
    }
}

bool quarantine_game::Utils::is_integer(string s) {
    try {
        stoi(s);
        return true;
    } catch (exception &e) {
        return false;
    }
}




quarantine_game::game_error::game_error(const string &what) : _what(what) {}

const char *quarantine_game::game_error::what() const noexcept {
    return _what.c_str();
}

