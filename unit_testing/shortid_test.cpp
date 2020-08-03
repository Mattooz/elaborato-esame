#include <gtest/gtest.h>
#include "../shortid.h"

using namespace quarantine_game;

/*
 * Using constant number to generate an id and test if the code worked.
 */
const uint64_t test =  9978055103041796096L;

TEST(shortid_test, id_test) {
    string id = ShortId::id(test);

    ASSERT_EQ("A4UOAs5KAQeOAo7K", id);
}
/*
 * Using known good id to test is_id method.
 */
const string good_id = "A4UOAs5KAQeOAo7K";

/*
 * Using known bad id to test is_id method.
 */
const string bad_id = "d4UgAw5KAQeOeo7K";

/*
 * This tests also the index_of(char c) method
 */
TEST(shortid_test, isid_test) {
    EXPECT_TRUE(ShortId::is_id(good_id));
    EXPECT_FALSE(ShortId::is_id(bad_id));
}

/*
 * This test the get_new_id() and get_new_id(string s) methods. Also tests implicitly get_random_long() method.
 */
TEST(shortid_test, getnewid_test) {
    //Good ids.
    string a = ShortId::get_new_id();
    string b = ShortId::get_new_id("test");

    //It is a requirement that the given string is longer than 4 characters in order to work.
    //This should then return a nullptr.
    string c = ShortId::get_new_id("tes");

    ASSERT_EQ(a.length(), 16);
    EXPECT_TRUE(ShortId::is_id(a));
    ASSERT_EQ(b.length(), 16);
    EXPECT_TRUE(ShortId::is_id(b));
    EXPECT_TRUE(c.empty());
}
