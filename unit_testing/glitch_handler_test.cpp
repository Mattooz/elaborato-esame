#include <gtest/gtest.h>
#include "../glitch_handler.h"
#include "../utils.h"
#include <nlohmann/json.hpp>

using namespace QuarantineGame;
using json = nlohmann::json;

TEST(GlitchHandlerSuite, test_lists) {
    auto a = GlitchHandler::lists();

    ASSERT_FALSE(a.empty());

    for (auto b : a) {
        string c;
        ASSERT_NO_THROW(c = b["name"].get<string>());
        ASSERT_FALSE(c.empty());

        ASSERT_NO_THROW(b["password"].get<string>());

        json d;

        ASSERT_NO_THROW(d = b["glitches"].get<json>());
    }
}

TEST(GlitchHandlerSuite, test_from_name) {
    auto a = GlitchHandler::from_name("default-list");
    auto b = GlitchHandler::from_name("not-a-list");

    ASSERT_TRUE(b.is_null());

    ASSERT_FALSE(a.is_null());

    string s;

    ASSERT_NO_THROW(s = a.at(2)["action"].get<string>());
    ASSERT_EQ(s, "roll-again");

    string s1;
    int i = 0;

    ASSERT_NO_THROW(s = a.at(2)["requires"].get<string>());
    ASSERT_NO_THROW(i = stoi(s));

    ASSERT_EQ(i, 1);
}

TEST(GlitchHandlerSuite, test_from_id) {
    auto a = GlitchHandler::from_id("dVPJAyAPVFPJAAPf");

    auto b = GlitchHandler::from_name("not-a-list");


    ASSERT_TRUE(b.is_null());

    ASSERT_FALSE(a.is_null());

    string s;

    ASSERT_NO_THROW(s = a.at(2)["action"].get<string>());
    ASSERT_EQ(s, "roll-again");

    string s1;
    int i = 0;

    ASSERT_NO_THROW(s = a.at(2)["requires"].get<string>());
    ASSERT_NO_THROW(i = stoi(s));

    ASSERT_EQ(i, 1);
}

TEST(GlitchHandlerSuite, test_list_names) {
    auto a = GlitchHandler::list_names();

    ASSERT_FALSE(a.empty());

    for (const auto &b : a) {
        ASSERT_FALSE(b.empty());
    }
}

/*
 * This checks also check_for_action_errors and check for players references.
 * Tests for every possible error.
 */
TEST(GlitchHandlerSuite, test_check_errors) {
    auto a = Utils::read_utf8_file(
            "/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/bad_json_glitch.json");

    auto b = json::parse(a);

    json c;

    c["1"] = GlitchHandler::check_for_errors(b["1"]);
    c["2"] = GlitchHandler::check_for_errors(b["2"]);
    c["3"] = GlitchHandler::check_for_errors(b["3"]);
    c["4"] = GlitchHandler::check_for_errors(b["4"]);
    c["5"] = GlitchHandler::check_for_errors(b["5"]);
    c["6"] = GlitchHandler::check_for_errors(b["6"]);

    auto d = Utils::read_utf8_file(
            "/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/bad_json_glitch_output.json");
    json e = json::parse(d);

    ASSERT_EQ(c, e);
}