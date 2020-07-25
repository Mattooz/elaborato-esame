//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "glitch.h"

void quarantine_game::action::operator+(const function<void()> &runnable) {
    runnables.push_back(runnable);
}

void quarantine_game::action::run() {
    for(auto & it : runnables)
        it();
}


quarantine_game::glitch::glitch(const string &message, const string &title, const vector<action> &actions,
                                const vector<string> &buttons) {}

const string &quarantine_game::glitch::_message() const {
    return message;
}

const string &quarantine_game::glitch::_title() const {
    return title;
}

const vector<string> &quarantine_game::glitch::_buttons() const {
    return buttons;
}

weak_ptr<quarantine_game::player> quarantine_game::glitch::get_player() const {
    return required[0];
}

const uint8_t quarantine_game::glitch::action_count() {
    return actions.size();
}

void quarantine_game::glitch::choose_action(uint8_t option) {
    if(option >= actions.size()) return;

    actions[option].run();
}
