//
// Created by Niccolò Mattei on 17/06/2020.
//

#include <string>
#include <iostream>
#include "shortid.h"

using namespace std;
using namespace quarantine_game;

const char shortid::chars[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                                 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
                                 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
                                 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', '-', '_'};

string shortid::id(uint64_t base) {
    uint8_t bytes[16];

    bytes[0] = (uint8_t) (base & 0x3F);
    bytes[1] = (uint8_t) (base >> 8 & 0x3F);
    bytes[2] = (uint8_t) (base >> 16 & 0x3F);
    bytes[3] = (uint8_t) (base >> 24 & 0x3F);
    bytes[4] = (uint8_t) (base >> 32 & 0x3F);
    bytes[5] = (uint8_t) (base >> 40 & 0x3F);
    bytes[6] = (uint8_t) (base >> 48 & 0x3F);
    bytes[7] = (uint8_t) (base >> 56 & 0x3F);
    bytes[8] = (uint8_t) (bytes[0] & bytes[1]);
    bytes[9] = (uint8_t) (bytes[1] & bytes[2]);
    bytes[10] = (uint8_t) (bytes[2] | bytes[3]);
    bytes[11] = (uint8_t) (bytes[3] | bytes[4]);
    bytes[12] = (uint8_t) (bytes[4] & bytes[5]);
    bytes[13] = (uint8_t) (bytes[5] & bytes[6]);
    bytes[14] = (uint8_t) (bytes[6] | bytes[7]);
    bytes[15] = (uint8_t) (bytes[7] | bytes[0]);

    string res;

    for (int i = 0; i < 16; i++) res += chars[bytes[i]];

    return res;
}

string shortid::get_new_id() {
    uint64_t nmb = get_random_long();

    return shortid::id(nmb);
}

string shortid::get_new_id(string text) {
    if (text.length() < 4) return "";

    uint64_t nmb = ((text.at(0) & 0xFFL) << 56) | ((text.at(0) & 0xFFL) << 48) |
                   ((text.at(2) & 0xFFL) << 40) | ((text.at(3) & 0xFFL) << 32) |
                   (get_random_long() & 0x7FFFFFFF);

    return shortid::id(nmb);
}

uint8_t shortid::index_of(char c) {
    for (uint8_t i = 0; i < 64; i++) if (shortid::chars[i] == c) return i;
    return 65;
}

bool shortid::is_id(string id) {
    if (id.length() != 16) return false;

    uint8_t bytes[16];

    for (int i = 0; i < 16; i++) {
        uint8_t ind = index_of(id.at(i));

        if (ind == 65) return false;
        bytes[i] = ind;
    }



    for (int i = 0; i < 8; i++) {
        switch (i) {
            case 0:
            case 1:
            case 4:
            case 5:
                if ((bytes[i] & bytes[((i + 1) % 8)]) != bytes[i + 8]) return false;
                break;
            case 2:
            case 3:
            case 6:
            case 7:
                if ((bytes[i] | bytes[((i + 1) % 8)]) != bytes[i + 8]) return false;
                break;
            default:
                return false;
        }
    }
    return true;
}


