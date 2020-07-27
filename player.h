//
// Created by Niccolò Mattei on 16/07/2020.
//

#include <string>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>

#ifndef ELABORATO_ESAME_PLAYER_H
#define ELABORATO_ESAME_PLAYER_H

using namespace std;
using json = nlohmann::json;

namespace quarantine_game {

    class player {
    private:
        string name;
        double money;
        uint8_t position;
        uint8_t turns_in_prison;
        int8_t blocked_for;
        int8_t avoid;
        vector<json> updates;
        bool has_quit;
        function<void()> quit;
    public:
        player() = delete;

        player(string name, double money, uint8_t position, function<void()> quit);

        const string &_name() const;

        double &_money();

        uint8_t & _position();

        uint8_t &_turns_in_prison();

        int8_t &_blocked_for();

        int8_t &_avoid();

        bool _has_quit() const;

        void add_update(const json &update);

        void delete_updates();

        json get_update();

        bool operator!=(player p);
        bool operator==(player p);
    };

}


#endif //ELABORATO_ESAME_PLAYER_H
