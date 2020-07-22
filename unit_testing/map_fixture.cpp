#include <gtest/gtest.h>
#include "../map.h"

using namespace quarantine_game;

class map_suite : public ::testing::Test {
protected:
    virtual void SetUp() {
        dynamic_cast<property_box*>(box9)->_owner() = 1;
        dynamic_cast<property_box*>(box10)->_owner() = 1;
    }

    box *box1 = new functional_box(0, "start");
    box *box2 = new property_box(1, 0, 50, "test-1");
    box *box3 = new property_box(2, 1, 50, "test-2");
    box *box4 = new property_box(3, 2, 50, "test-3");
    box *box5 = new functional_box(4, "glitch");
    box *box6 = new property_box(5, 3, 50, "test-4");
    box *box7 = new property_box(6, 4, 50, "test-5");
    box *box8 = new functional_box(7, "glitch");
    box *box9 = new property_box(8, 5, 50, "test-6");
    box *box10 = new property_box(9, 6, 50, "test-7");
    box *box11 = new functional_box(10, "goto-prison");
    box *box12 = new property_box(11, 7, 50, "test-8");
    box *box13 = new property_box(12, 8, 50, "test-9");

    quarantine_game::map map{{box1, box2, box3, box4, box5, box6, box7, box8, box9, box10, box11, box12, box13}};

};

TEST_F(map_suite, test_cost) {
    ASSERT_EQ(map.cost(2), 50);
    ASSERT_EQ(map.cost(10), -1);
}

TEST_F(map_suite, test_name) {
    ASSERT_EQ(map.name(2), "test-2");
    ASSERT_EQ(map.name(10), "not found");
}

TEST_F(map_suite, test_id) {
    ASSERT_EQ(map.id(9), 6);
    ASSERT_EQ(map.id(10), 0xFF);
}

TEST_F(map_suite, test_pos) {
    ASSERT_EQ(map.pos(7), 11);
    ASSERT_EQ(map.pos(9), 0xFF);
}

TEST_F(map_suite, test_op_pos) {
    auto it = map[10];
    auto it1 = map[11];

    auto functional = dynamic_cast<functional_box *>(it);
    string type;

    if(functional) type = functional->_type();

    EXPECT_TRUE(functional);
    ASSERT_EQ(type, "goto-prison");

    auto property = dynamic_cast<property_box *>(it1);
    string name;
    uint8_t id = 0xFF;
    int32_t cost = -1;

    if(property) {
        name = property->_name();
        id = property->_id();
        cost = property->_cost();
    }

    EXPECT_TRUE(property);
    ASSERT_EQ(name, "test-8");
    ASSERT_EQ(id, 7);
    ASSERT_EQ(cost, 50);
}

TEST_F(map_suite, test_op_name) {
    auto it = map["test-2"];
    auto it1 = map["test not found"];

    auto property = dynamic_cast<property_box *>(it);

    string name;
    uint8_t id = 0xFF;
    int32_t cost = -1;

    if(property) {
        name = property->_name();
        id = property->_id();
        cost = property->_cost();
    }

    EXPECT_TRUE(property);
    ASSERT_EQ(name, "test-2");
    ASSERT_EQ(id, 1);
    ASSERT_EQ(cost, 50);

    EXPECT_FALSE(it1);
}

TEST_F(map_suite, test_fromid) {
    auto it = map.from_id(1);
    auto it1 = map.from_id(9);

    auto property = dynamic_cast<property_box *>(it);

    string name;
    uint8_t id = 0xFF;
    int32_t cost = -1;

    if(property) {
        name = property->_name();
        id = property->_id();
        cost = property->_cost();
    }

    EXPECT_TRUE(property);
    ASSERT_EQ(name, "test-2");
    ASSERT_EQ(id, 1);
    ASSERT_EQ(cost, 50);

    EXPECT_FALSE(it1);
}

TEST_F(map_suite, test_get_distance_to_glitch) {
    ASSERT_EQ(map.distance_to_next_glitch(9), 8);
    ASSERT_EQ(map.distance_to_next_glitch(13), 0xFF);
}

TEST_F(map_suite, test_get_distance_to_prison) {
    ASSERT_EQ(map.distance_to_prison(8), 2);
    ASSERT_EQ(map.distance_to_prison(13), 0xFF);
}

TEST_F(map_suite, test_get_properties) {
    auto it = map.get_player_properties(1);

    ASSERT_EQ(it.size(), 2);
}

TEST_F(map_suite, test_delete_properties) {
    map.delete_player_properties(1);
    auto it = map.get_player_properties(1);

    ASSERT_EQ(it.size(), 0);
}

TEST_F(map_suite, test_isglitch) {
    EXPECT_TRUE(map.is_glitch(4));
    EXPECT_FALSE(map.is_glitch(6));
}

TEST_F(map_suite, test_isprison) {
    EXPECT_TRUE(map.is_prison(10));
    EXPECT_FALSE(map.is_prison(6));
}