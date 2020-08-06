//
// Created by Niccolò Mattei on 03/08/2020.
//

#include <gtest/gtest.h>
#include "../game.h"
#include "../glitch_handler.h"

using namespace quarantine_game;

class Glitch_factory_suite : public ::testing::Test {
protected:
    virtual void SetUp() {
        dummy_game.add_player("Giocatore-2");
        dummy_game.add_player("Giocatore-3");
        dummy_game.add_player("Giocatore-4");
    }

    bool random_number() {
        int32_t previous = -1;

        //Running test for an arbitrary amount of times
        for (int i = 0; i < 30; i++) {
            int32_t current = factory.get_random_num();

            if (previous == current) return false;

            previous = current;
        }

        return true;
    }

    void random_glitch() {
        //Running test for an arbitrary amount of times
        for (int i = 0; i < 30; i++) {
            auto a = dummy_game.get_game_container();

            //This throws an exception if for some reason the random number is out of bounds of the array
            auto b = factory.get_random_glitch(a);

            //These throw an exception if it fails to cast to the correct type
            b["buttons"].get<json>();
            b["action"].get<string>();
            b["message"].get<string>();
            b["requires"].get<string>();
        }
    }

    bool check_good_action() {
        auto a = "pay rnd";
        auto b = dummy_game.get_game_container();

        factory.building.required = {dummy_game.get_player(0)};
        factory.parse_action(a, b);

        return factory.building.actions[0].unknown;
    }

    bool check_bad_action() {
        auto a = "unknown rnd";
        auto b = dummy_game.get_game_container();

        factory.building.required = {dummy_game.get_player(0)};
        factory.parse_action(a, b);

        return factory.building.actions[1].unknown;
    }

    bool check_get_random_player() {
        auto a = dummy_game.get_game_container();

        for (int i = 0; i < 5; i++)
            if (factory.get_random_player(0, a) == 0) return false;
        return true;
    }


    Game dummy_game{"Giocatore-1", 2000, "default-list", "default-map"};
    GlitchFactory &factory = dummy_game.factory;
};

TEST_F(Glitch_factory_suite, test_random_number) {
    ASSERT_TRUE(random_number());
}

TEST_F(Glitch_factory_suite, test_random_glitch) {
    ASSERT_NO_THROW(random_glitch());
}

TEST_F(Glitch_factory_suite, test_valid_action) {
    ASSERT_FALSE(check_good_action());
    ASSERT_TRUE(check_bad_action());
}

TEST_F(Glitch_factory_suite, test_random_player) {
    ASSERT_NO_THROW(random_glitch());
}

TEST_F(Glitch_factory_suite, test_valid_rnd_player) {
    ASSERT_TRUE(check_get_random_player());
}

TEST_F(Glitch_factory_suite, test_valid_pay_glitch) {
    auto a = dummy_game.get_game_container();
    auto b = factory.glitch(0, a, 15);

    b.choose_action(0);

    ASSERT_EQ(b.get_player().lock()->_money(), 1800);
}

TEST_F(Glitch_factory_suite, test_valid_cashin_glitch) {
    auto a = dummy_game.get_game_container();
    auto b = factory.glitch(0, a, 18);

    b.choose_action(0);

    ASSERT_EQ(b.get_player().lock()->_money(), 2200);
}

TEST_F(Glitch_factory_suite, test_valid_cashin_rnd_glitch) {
    auto a = dummy_game.get_game_container();
    auto b = factory.glitch(0, a, 21);

    b.choose_action(0);

    /*
     * The random amount of money can go from 0 to 50%. If it is 0 then
     * the amount of money won't change.
     */
    EXPECT_GE(b.get_player().lock()->_money(), 2000);
}

TEST_F(Glitch_factory_suite, test_valid_blocked_and_pay_rnd_glitch) {
    auto a = dummy_game.get_game_container();
    auto b = factory.glitch(0, a, 16);

    b.choose_action(0);

    ASSERT_LT(b.get_player().lock()->_money(), 2000);
    ASSERT_GE(b.get_player().lock()->_blocked_for(), 1);
}

TEST_F(Glitch_factory_suite, test_valid_get_transaction_glitch) {
    auto a = dummy_game.get_game_container();
    auto b = factory.glitch(0, a, 17);

    b.choose_action(0);

    ASSERT_EQ(*a.redirect_to, 0);
}

TEST_F(Glitch_factory_suite, test_valid_can_roll_again_glitch) {
    auto a = dummy_game.get_game_container();
    auto b = factory.glitch(0, a, 2);

    b.choose_action(0);

    ASSERT_EQ(*a.can_roll_again, 0);
}

TEST_F(Glitch_factory_suite, test_goto_prison) {
    auto a = dummy_game.get_game_container();
    auto b = factory.goto_prison(0, a);

    b.choose_action(0);

    ASSERT_EQ(b.get_player().lock()->_position(), 9);
    ASSERT_EQ(b.get_player().lock()->_turns_in_prison(), 3);
}

TEST_F(Glitch_factory_suite, test_empty_glitch) {
    auto a = quarantine_game::GlitchFactory::empty_glitch();

    ASSERT_EQ(a._title(), "empty");
    ASSERT_EQ(a._message(), "empty");
}