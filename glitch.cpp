//
// Created by Niccolò Mattei on 23/07/2020.
//

#include "glitch.h"
#include <iostream>

void QuarantineGame::Action::operator+(const function<void()> &runnable) {
    runnables.push_back(runnable);
}

void QuarantineGame::Action::run() {
    for (auto &it : runnables)
        it();
}


QuarantineGame::Glitch::Glitch(const string &message, const string &title, const vector<Action> &actions,
                               const vector<string> &buttons, uint8_t requires) : message(message), title(title),
                                                                                   actions(actions),
                                                                                   buttons(buttons),
                                                                                   requires(requires){}

const string &QuarantineGame::Glitch::_message() const {
    return message;
}

const string &QuarantineGame::Glitch::_title() const {
    return title;
}

const vector<string> &QuarantineGame::Glitch::_buttons() const {
    return buttons;
}

shared_ptr<QuarantineGame::Player> QuarantineGame::Glitch::get_player() const {
    return required[0];
}

const uint8_t QuarantineGame::Glitch::action_count() {
    return actions.size();
}

void QuarantineGame::Glitch::choose_action(uint8_t option) {
    if (option >= actions.size())
        return;


    actions[option].run();
}
