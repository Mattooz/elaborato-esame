//
// Created by Niccolò Mattei on 05/08/2020.
//

#include <gtest/gtest.h>
#include "../game.h"

using namespace quarantine_game;

class GameSuite : public ::testing::Test {
protected:
    virtual void SetUp() {
        test_game.add_player("Giocatore-1");
        test_game.add_player("Giocatore-2");
        test_game.add_player("Giocatore-3");
        test_game.add_player("Giocatore-4");
    }

    Game test_game{"Giocatore Host", 2000, "default-list", "default-map"};

};

TEST_F(GameSuite, test_get_players) {
    for (const auto &a : test_game.get_players()) {
        ASSERT_FALSE(a.expired());
    }
}

TEST_F(GameSuite, test_start) {
    ASSERT_FALSE(test_game.started());
    test_game.start();
    ASSERT_TRUE(test_game.started());

    for (auto a : test_game.get_players()) {
        auto b = a.lock();

        ASSERT_TRUE(b->get_update()["isNew"].get<bool>());
        ASSERT_FALSE(b->get_update()["isNew"].get<bool>());
    }
}

TEST_F(GameSuite, test_full) {
    ASSERT_FALSE(test_game.full());
    test_game.add_player("Giocatore-5");
    ASSERT_TRUE(test_game.full());
}

TEST_F(GameSuite, test_get_player_from_turn) {
    auto p = test_game.get_player(0);

    ASSERT_FALSE(p.expired());

    auto p1 = test_game.get_player(6);

    ASSERT_TRUE(p1.expired());
}

TEST_F(GameSuite, test_get_player_from_name) {
    auto p = test_game.get_player("Giocatore Host");

    ASSERT_FALSE(p.expired());

    auto p1 = test_game.get_player("Not-a-player");

    ASSERT_TRUE(p1.expired());
}

TEST_F(GameSuite, test_get_player_turn) {
    auto p = test_game.get_player_turn("Giocatore Host");

    ASSERT_EQ(p, 0);

    auto p1 = test_game.get_player_turn("Not-a-player");

    ASSERT_EQ(p1, Game::not_found);
}

TEST_F(GameSuite, test_move_player_normal) {
    auto a = test_game.get_player(0).lock();

    ASSERT_NO_THROW(test_game.move_player(0, test_game.get_player(0), 3, 3, 6, false));

    ASSERT_EQ(a->_position(), 0);
    ASSERT_FALSE(a->get_update()["isNew"].get<bool>());

    test_game.start();

    //Discard first update
    a->get_update();

    ASSERT_NO_THROW(test_game.move_player(1, test_game.get_player(1), 3, 3, 6, false));

    auto b = test_game.get_player(1).lock();

    ASSERT_EQ(b->_position(), 0);
    ASSERT_FALSE(a->get_update()["isNew"].get<bool>());

    ASSERT_NO_THROW(test_game.move_player(0, test_game.get_player(0), 3, 3, 6, false));

    json upd = a->get_update();

    ASSERT_EQ(a->_position(), 6);
    ASSERT_TRUE(upd["isNew"].get<bool>());
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice1"], 3));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice2"], 3));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerPos"], 6));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerNum"], 0));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["instant"], false));

    ASSERT_EQ(a->_money(), 2000);
}

TEST_F(GameSuite, test_move_player_on_property) {
    auto a = test_game.get_player(0).lock();

    test_game.map()["Coverciano"].lock()->_owner() = 1;
    test_game.map()["Coverciano"].lock()->_houses() = 1;

    test_game.start();

    //Discard first update
    a->get_update();


    ASSERT_NO_THROW(test_game.move_player(0, test_game.get_player(0), 3, 3, 6, false));

    auto b = test_game.get_player(1).lock();

    json upd = a->get_update();

    ASSERT_EQ(a->_position(), 6);
    ASSERT_TRUE(upd["isNew"].get<bool>());
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice1"], 3));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice2"], 3));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerPos"], 6));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerNum"], 0));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["instant"], false));

    ASSERT_EQ(a->_money(), 1850);
    ASSERT_EQ(b->_money(), 2150);
}

TEST_F(GameSuite, test_move_player_on_property_with_redirect) {
    auto a = test_game.get_player(0).lock();

    test_game.map()["Coverciano"].lock()->_owner() = 1;
    test_game.map()["Coverciano"].lock()->_houses() = 1;

    test_game.redirect() = 2;

    test_game.start();
    //Discard first update
    a->get_update();

    ASSERT_NO_THROW(test_game.move_player(0, test_game.get_player(0), 3, 3, 6, false));

    auto b = test_game.get_player(2).lock();

    json upd = a->get_update();

    ASSERT_EQ(a->_position(), 6);
    ASSERT_TRUE(upd["isNew"].get<bool>());
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice1"], 3));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice2"], 3));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerPos"], 6));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerNum"], 0));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["instant"], false));

    ASSERT_EQ(a->_money(), 1850);
    ASSERT_EQ(b->_money(), 2150);
}

TEST_F(GameSuite, test_move_player_on_glitch) {
    auto a = test_game.get_player(0).lock();

    test_game.start();
    //Discard first update
    a->get_update();

    ASSERT_NO_THROW(test_game.move_player(0, test_game.get_player(0), 4, 4, 8, false));

    json upd = a->get_update();

    ASSERT_EQ(a->_position(), 8);
    ASSERT_TRUE(upd["isNew"].get<bool>());
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice1"], 4));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice2"], 4));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerPos"], 8));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerNum"], 0));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["instant"], false));

    ASSERT_FALSE(test_game.current_glitch()._title() == "empty");
    ASSERT_FALSE(test_game.current_glitch()._message() == "empty");
}

TEST_F(GameSuite, test_move_player_on_glitch_with_avoid) {
    auto a = test_game.get_player(0).lock();

    a->_avoid() = 3;

    test_game.start();
    //Discard first update
    a->get_update();

    ASSERT_NO_THROW(test_game.move_player(0, test_game.get_player(0), 4, 4, 8, false));

    json upd = a->get_update();

    ASSERT_EQ(a->_position(), 8);
    ASSERT_TRUE(upd["isNew"].get<bool>());
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice1"], 4));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["dice2"], 4));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerPos"], 8));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["playerNum"], 0));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePlayerPos"].at(0)["instant"], false));

    ASSERT_TRUE(test_game.current_glitch()._title() == "empty");
    ASSERT_TRUE(test_game.current_glitch()._message() == "empty");
    ASSERT_EQ(a->_avoid(), 2);
}

TEST_F(GameSuite, test_roll_dice_and_move_normal) {
    auto a = test_game.get_player(0).lock();

    test_game.roll_dice(0);

    ASSERT_EQ(a->_position(), 0);
    ASSERT_FALSE(a->get_update()["isNew"].get<bool>());

    test_game.start();
    //Discard first update
    a->get_update();

    test_game.roll_dice(1);

    auto b = test_game.get_player(1).lock();

    ASSERT_EQ(b->_position(), 0);
    ASSERT_FALSE(a->get_update()["isNew"].get<bool>());

    test_game.roll_dice(0);

    json upd = a->get_update();

    int dice1 = 0, dice2 = 0, new_pos = 0;

    ASSERT_NO_THROW(dice1 = upd["updatePlayerPos"].at(0)["dice1"].get<int>());
    ASSERT_NO_THROW(dice2 = upd["updatePlayerPos"].at(0)["dice2"].get<int>());
    ASSERT_NO_THROW(new_pos = upd["updatePlayerPos"].at(0)["playerPos"].get<int>());

    /*
     * We can do this as the player starts from position 0
     */
    ASSERT_EQ((dice1 + dice2), new_pos);
    ASSERT_EQ(a->_position(), new_pos);
}

TEST_F(GameSuite, test_roll_dice_and_move_in_prison) {
    auto a = test_game.get_player(0).lock();
    a->_turns_in_prison() = 3;

    test_game.start();
    //Discard first update
    a->get_update();

    test_game.roll_dice(0);

    json upd = a->get_update();

    int dice1 = 0, dice2 = 0, new_pos = 0;


    ASSERT_TRUE(upd["isNew"].get<bool>());

    /*
     * Since the dices are random, I check if the player has escaped successfully or not
     * If the two dices are the same then I assume that the player has escaped.
     */
    if (upd.contains("dice1")) {
        ASSERT_NO_THROW(dice1 = upd["dice1"].get<int>());
        ASSERT_NO_THROW(dice2 = upd["dice2"].get<int>());
    } else {
        ASSERT_NO_THROW(dice1 = upd["updatePlayerPos"].at(0)["dice1"].get<int>());
        ASSERT_NO_THROW(dice2 = upd["updatePlayerPos"].at(0)["dice2"].get<int>());
        ASSERT_NO_THROW(new_pos = upd["updatePlayerPos"].at(0)["playerPos"].get<int>());
    }

    /*
     * Here I assume that prison is on position 0.
     * I do this to make my life easier.
     */
    if (dice1 == dice2) {
        ASSERT_EQ((dice1 + dice2), new_pos);
        ASSERT_EQ(a->_position(), new_pos);
        ASSERT_EQ(a->_turns_in_prison(), 0);
    } else {
        ASSERT_EQ(a->_position(), 0);
        ASSERT_EQ(a->_turns_in_prison(), 2);
        ASSERT_FALSE(test_game.current_glitch()._title() == "empty");
        ASSERT_FALSE(test_game.current_glitch()._message() == "empty");
    }
}

TEST_F(GameSuite, test_roll_dice_and_move_with_negative_money) {
    auto a = test_game.get_player(0).lock();
    /*
     * Arbitrary amount of negative money
     */
    a->_money() = -100;

    test_game.start();
    //Discard first update
    a->get_update();

    test_game.roll_dice(0);

    json upd = a->get_update();

    int dice1 = 0, dice2 = 0, new_pos = 0;

    ASSERT_NO_THROW(dice1 = upd["updatePlayerPos"].at(0)["dice1"].get<int>());
    ASSERT_NO_THROW(dice2 = upd["updatePlayerPos"].at(0)["dice2"].get<int>());
    ASSERT_NO_THROW(new_pos = upd["updatePlayerPos"].at(0)["playerPos"].get<int>());

    /*
     * A player with negative money should land on a glitch.
     */
    ASSERT_EQ((dice1 + dice2), new_pos);
    ASSERT_EQ(a->_position(), new_pos);

    ASSERT_FALSE(test_game.current_glitch()._title() == "empty");
    ASSERT_FALSE(test_game.current_glitch()._message() == "empty");
}

TEST_F(GameSuite, test_roll_dice_and_move_with_debug_goto_prison) {
    auto a = test_game.get_player(0).lock();

    test_game.goto_prison = true;

    test_game.start();
    //Discard first update
    a->get_update();

    test_game.roll_dice(0);

    json upd = a->get_update();

    int dice1 = 0, dice2 = 0, new_pos = 0;

    ASSERT_NO_THROW(dice1 = upd["updatePlayerPos"].at(0)["dice1"].get<int>());
    ASSERT_NO_THROW(dice2 = upd["updatePlayerPos"].at(0)["dice2"].get<int>());
    ASSERT_NO_THROW(new_pos = upd["updatePlayerPos"].at(0)["playerPos"].get<int>());

    /*
     * Good enough. It should be 12 from position 0.
     */
    ASSERT_EQ(dice1, 6);
    ASSERT_EQ(dice2, 6);
    ASSERT_EQ(a->_position(), 12);
}

TEST_F(GameSuite, test_next_player_normal) {
    test_game.next_player(0);
    ASSERT_EQ(test_game._turns(), 0);

    test_game.start();


    test_game.next_player(1);
    ASSERT_EQ(test_game._turns(), 0);

    test_game.next_player(0);
    ASSERT_EQ(test_game._turns(), 1);
}

TEST_F(GameSuite, test_next_player_blocked) {
    auto a = test_game.get_player(1).lock();

    a->_blocked_for() = 3;

    test_game.start();

    test_game.next_player(0);
    ASSERT_EQ(test_game._turns(), 2);
    ASSERT_EQ(a->_blocked_for(), 2);
}

TEST_F(GameSuite, test_next_player_can_roll_again) {
    test_game.roll_again() = 0;

    test_game.start();

    test_game.next_player(0);
    ASSERT_EQ(test_game._turns(), 0);
    ASSERT_EQ(test_game.roll_again(), -1);
}

TEST_F(GameSuite, test_buy_property_normal) {
    auto a = test_game.get_player(0).lock();

    ASSERT_NO_THROW(test_game.buy_property(2, 0));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_owner(), Map::not_found);

    test_game.start();

    //Discard first update
    a->get_update();

    ASSERT_NO_THROW(test_game.buy_property(2, 1));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_owner(), Map::not_found);

    ASSERT_NO_THROW(test_game.buy_property(2, 0));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_owner(), Map::not_found);
    ASSERT_EQ(a->_money(), 2000);

    a->_position() = 4;

    ASSERT_NO_THROW(test_game.buy_property(2, 0));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_owner(), 0);
    ASSERT_EQ(a->_money(), 1900);

    json upd = a->get_update();

    //Reset the map. Apparently the Map doesn't get reset after each test.
    test_game.map().reset();

    ASSERT_NO_THROW(ASSERT_TRUE(upd["isNew"].get<bool>()));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePropertyColor"].at(0)["pos"].get<int>(), 2));
    ASSERT_NO_THROW(ASSERT_EQ(upd["updatePropertyColor"].at(0)["player"].get<int>(), 0));
}

TEST_F(GameSuite, test_buy_property_not_enough_money) {
    auto a = test_game.get_player(0).lock();
    a->_money() = 50;

    test_game.start();

    //Discard first update
    a->get_update();
    ASSERT_NO_THROW(test_game.buy_property(2, 0));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_owner(), Map::not_found);
    ASSERT_EQ(a->_money(), 50);

    json upd = a->get_update();

    //Reset the map. Apparently the Map doesn't get reset after each test.
    test_game.map().reset();

    ASSERT_NO_THROW(ASSERT_FALSE(upd["isNew"].get<bool>()));
    ASSERT_FALSE(upd.contains("updatePropertyColor"));
}

TEST_F(GameSuite, test_buy_house_normal) {
    auto a = test_game.get_player(0).lock();
    auto b = test_game.get_player(1).lock();

    test_game.map().from_id(2).lock()->_owner() = 0;
    test_game.map().from_id(3).lock()->_owner() = 1;

    ASSERT_NO_THROW(test_game.buy_house(0, 2, 3));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_houses(), 0);
    ASSERT_EQ(a->_money(), 2000);

    ASSERT_NO_THROW(test_game.buy_house(1, 3, 3));
    ASSERT_EQ(test_game.map().from_id(3).lock()->_houses(), 0);
    ASSERT_EQ(b->_money(), 2000);

    test_game.start();

    ASSERT_NO_THROW(test_game.buy_house(0, 2, 3));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_houses(), 3);
    ASSERT_EQ(a->_money(), 1850);

    ASSERT_NO_THROW(test_game.buy_house(1, 3, 3));
    ASSERT_EQ(test_game.map().from_id(3).lock()->_houses(), 0);
    ASSERT_EQ(b->_money(), 2000);

    ASSERT_NO_THROW(test_game.buy_house(0, 2, 2));
    ASSERT_EQ(test_game.map().from_id(2).lock()->_houses(), 2);
    ASSERT_EQ(a->_money(), 1875);

    //Reset the map. Apparently the Map doesn't get reset after each test.
    test_game.map().reset();
}

/*
 * Includes also remove_player
 */
TEST_F(GameSuite, test_player_quit_not_started) {
    test_game.player_quit(0);

    ASSERT_EQ(test_game.player_count(), 4);
    ASSERT_EQ(test_game.get_player(0).lock()->_name(), "Giocatore-1");

    auto a = test_game.get_player(0).lock();

    json upd = a->get_update();

    ASSERT_NO_THROW(ASSERT_TRUE(upd["isNew"].get<bool>()));
    ASSERT_NO_THROW(ASSERT_EQ(upd["playerQuit"]["playerId"], 0));
    ASSERT_NO_THROW(ASSERT_EQ(upd["playerQuit"]["newTurn"], 0));
}

TEST_F(GameSuite, test_player_quit_started) {
    test_game.map().from_id(2).lock()->_owner() = 0;
    test_game.map().from_id(3).lock()->_owner() = 0;

    test_game.start();

    auto a = test_game.get_player(1).lock();
    //Discard first update
    a->get_update();

    test_game.player_quit(0);

    //Discard next_player update
    a->get_update();


    ASSERT_EQ(test_game.player_count(), 5);
    ASSERT_EQ(test_game.get_player(0).lock()->_name(), "Giocatore Host");
    ASSERT_EQ(test_game._turns(), 1);

    auto b = test_game.get_offline_player("Giocatore Host");

    ASSERT_FALSE(b.expired());

    ASSERT_EQ(test_game.map().from_id(2).lock()->_owner(), Map::not_found);
    ASSERT_EQ(test_game.map().from_id(3).lock()->_owner(), Map::not_found);

    json upd = a->get_update();

    ASSERT_NO_THROW(ASSERT_TRUE(upd["isNew"].get<bool>()));
    ASSERT_NO_THROW(ASSERT_EQ(upd["playerQuit"]["playerId"], 0));
    ASSERT_NO_THROW(ASSERT_EQ(upd["playerQuit"]["newTurn"], -1));
}

TEST_F(GameSuite, test_player_rejoin) {
    test_game.map().from_id(2).lock()->_owner() = 1;
    test_game.map().from_id(3).lock()->_owner() = 1;

    test_game.start();

    auto a = test_game.get_player(1).lock();
    //Discard first update
    a->get_update();

    test_game.player_quit(0);

    //Discard quit update and next_player update
    a->get_update();
    a->get_update();

    test_game.player_rejoin("Giocatore Host");

    auto b = test_game.get_player(0).lock();

    auto c = test_game.get_offline_player("Giocatore Host");

    ASSERT_TRUE(c.expired());

    json upd = a->get_update();

    json upd1 = b->get_update();

    ASSERT_NO_THROW(ASSERT_TRUE(upd["isNew"].get<bool>()));
    ASSERT_NO_THROW(ASSERT_EQ(upd["playerRejoined"], 0));

    ASSERT_NO_THROW(ASSERT_TRUE(upd1["isNew"].get<bool>()));
    ASSERT_NO_THROW(ASSERT_EQ(upd1["updatePlayerPos"].size(), 5));
    ASSERT_NO_THROW(ASSERT_EQ(upd1["updateHouseCount"].size(), 2));
    ASSERT_NO_THROW(ASSERT_EQ(upd1["updatePropertyColor"].size(), 2));
}