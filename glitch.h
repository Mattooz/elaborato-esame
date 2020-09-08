#include <vector>
#include <functional>
#include <string>
#include <nlohmann/json.hpp>
#include "player.h"
#include "update_builder.h"

#ifndef ELABORATO_ESAME_GLITCH_H
#define ELABORATO_ESAME_GLITCH_H

using namespace std;

class glitch_suite;
class Glitch_factory_suite;

namespace QuarantineGame {
    class Action {
    private:
        vector<function<void()>> runnables;
    public:
        bool unknown;

        Action() : unknown(false) {};

        void operator+(const function<void()> &runnable);

        void run();
    };

    class Glitch {
    private:
        friend class GlitchFactory;
        friend class ::glitch_suite;
        friend class ::Glitch_factory_suite;

        string message;
        string title;
        vector<shared_ptr<Player>> required;
        vector<Action> actions;
        vector<string> buttons;
        uint8_t requires;
    public:
        Glitch(const string &message, const string &title, const vector<Action> &actions, const vector<string> &buttons,
               uint8_t requires);
        const string &_message() const;
        const string &_title() const;
        const vector<string> &_buttons() const;
        shared_ptr<Player> get_player() const;
        const uint8_t action_count();
        void choose_action(uint8_t option);

    };

}


#endif //ELABORATO_ESAME_GLITCH_H
