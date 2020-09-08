
#include <gtest/gtest.h>
#include "../utils.h"
#include <string>

using namespace QuarantineGame::Utils;

TEST(UtilsTest, get_random_long) {
    auto a = get_random_long();

    ASSERT_TRUE(a >= numeric_limits<uint64_t>::min() && a <= numeric_limits<uint64_t>::max());
}

TEST(UtilsTest, get_random_num) {
    auto a = get_random_num(10);

    ASSERT_TRUE(a >= 0 && a <= 10);
}

TEST(UtilsTest, get_random_dice) {
    auto a = get_random_dice();

    ASSERT_TRUE(a >= 1 && a <= 6);
}

TEST(UtilsTest, read_file) {
    auto a = read_file("/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/test_ascii.txt");

    ASSERT_EQ(a, "This is a test file");

    auto b = read_file("not/a/valid/path.txt");

    ASSERT_EQ(b, "not found");
}

TEST(UtilsTest, read_utf8_file) {
    auto a = read_utf8_file("/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/test_utf8.txt");

    wstring b = L"This is test utf-8 file, àèìòù";

    ASSERT_EQ(a, b);

    auto c = read_utf8_file("not/a/valid/path.txt");

    ASSERT_EQ(c, L"not found");
}

TEST(UtilsTest, test_split) {
    auto a = "A,B,C";

    auto b = split(a, ",");

    ASSERT_EQ(b.size(), 3);
    ASSERT_EQ(b[0], "A");
    ASSERT_EQ(b[1], "B");
    ASSERT_EQ(b[2], "C");
}

TEST(UtilsTest, test_is_integer) {
    auto a = "1", b = "d";

    ASSERT_TRUE(is_integer(a));
    ASSERT_FALSE(is_integer(b));
}

TEST(UtilsTest, test_is_number) {
    auto a = "3.14", b = "d";

    ASSERT_TRUE(is_number(a));
    ASSERT_FALSE(is_number(b));
}



