#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include "../glitch.h"
#include "../glitch_handler.h"


#define LAMBDA_FUNCTION_DECL [&]()

using namespace std;
using namespace quarantine_game;

class glitch_suite : public ::testing::Test {
protected:
    virtual void SetUp() {
        c.actions[0] + LAMBDA_FUNCTION_DECL {
            test += "TEST 1";
            a = a - b;
        };

        c.actions[1] + LAMBDA_FUNCTION_DECL {
            test += "TEST 2";
            a = a + b;
        };
    }

    string test;
    int a = 7;
    int b = 9;

    vector<string> buttons{"test1", "test2"};

    action action1;
    action action2;

    vector<action> actions {action1, action2};

    glitch c{"test message", "test title", actions, buttons};
};

TEST_F(glitch_suite, test_chose_action1) {
    //Choosing action 1
    c.choose_action(0);

    ASSERT_EQ(test, "TEST 1");
    ASSERT_EQ(a, -2);
}

TEST_F(glitch_suite, test_chose_action2) {
    //Choosing action 2
    c.choose_action(1);

    ASSERT_EQ(test, "TEST 2");
    ASSERT_EQ(a, 16);
}

