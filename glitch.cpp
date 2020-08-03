//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "glitch.h"
#include <iostream>

void quarantine_game::Action::operator+(const function<void()> &runnable) {
    runnables.push_back(runnable);
}

void quarantine_game::Action::run() {
    for (auto &it : runnables)
        it();
}


quarantine_game::Glitch::Glitch(const string &message, const string &title, const vector<Action> &actions,
                                const vector<string> &buttons, uint8_t requires) : message(message), title(title),
                                                                                   actions(actions),
                                                                                   buttons(buttons),
                                                                                   requires(requires){}

const string &quarantine_game::Glitch::_message() const {
    return message;
}

const string &quarantine_game::Glitch::_title() const {
    return title;
}

const vector<string> &quarantine_game::Glitch::_buttons() const {
    return buttons;
}

weak_ptr<quarantine_game::Player> quarantine_game::Glitch::get_player() const {
    return required[0];
}

const uint8_t quarantine_game::Glitch::action_count() {
    return actions.size();
}

void quarantine_game::Glitch::choose_action(uint8_t option) {
    if (option >= actions.size())
        return;


    actions[option].run();
}
