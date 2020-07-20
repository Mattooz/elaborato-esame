#include <gtest/gtest.h>
#include "../update_builder.h"
#include <string>
#include "../utils.h"


using namespace quarantine_game;
using namespace std;

/*
 * Correct json output generated with the Java version of the builder.
 */
const string correct_json_game_update = utils::read_file(
        "/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/game_builder_json_output.json");

class game_upd_bld_suite : public ::testing::Test {
protected:
    virtual void SetUp() {
        builder1->start()
                ->move(4, 5, 9, 1, false)
                ->color(18, 1)
                ->house_count(18, 4)
                ->new_glitch("Testing new glitch", "Dummy title", {"test", "foo", "bar"})
                ->other("bool", true)
                ->other("unsigned int", ((uint64_t) 19))
                ->other("signed int", ((int64_t) -19))
                ->other("double", 2.49)
                ->other("string", "foobar");
    }

    /*
     * Setting up dummy update_game_container.
     */
    player p1{"player1", 2000, 0, nullptr}; //We don't need the quit function for this test
    player p2{"player2", 2000, 0, nullptr};
    player p3{"player3", 2000, 0, nullptr};

    vector<player> players{p1, p2, p3};
    bool hasStarted = true;
    uint32_t turns = 29;

    update_game_container container{&players, &turns, &hasStarted};

    game_update_builder *builder1 = new game_update_builder(container);
};

TEST_F(game_upd_bld_suite, test_result_json) {
    ASSERT_EQ(builder1->res().dump(), correct_json_game_update);

    delete builder1;
}