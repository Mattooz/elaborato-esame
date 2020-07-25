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

namespace quarantine_game {
    class action {
    private:
        vector<function<void()>> runnables;
    public:
        action() = default;

        void operator+(const function<void()> &runnable);

        void run();
    };

    class glitch {
    private:
        friend class glitch_factory;
        friend class ::glitch_suite;

        string message;
        string title;
        vector<weak_ptr<player>> required;
        vector<action> actions;
        vector<string> buttons;
    public:
        glitch(const string &message, const string &title, const vector<action> &actions, const vector<string> &buttons);
        const string &_message() const;
        const string &_title() const;
        const vector<string> &_buttons() const;
        weak_ptr<player> get_player() const;
        const uint8_t action_count();
        void choose_action(uint8_t option);

    };




}


#endif //ELABORATO_ESAME_GLITCH_H
