#include "../update_builder.h"
#include "../utils.h"
#include "../map_factory.h"
#include "../map.h"
#include <string>
#include <gtest/gtest.h>

using namespace quarantine_game;
using namespace std;

/*
 * Correct json output generated with the Java version of the builder.
 */
const string correct_json_game_update = Utils::read_file(
        "/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/game_builder_json_output.json");

class game_upd_bld_suite : public ::testing::Test {
protected:
    game_upd_bld_suite() {}

    virtual void SetUp() {
        map["Ponticello"].lock()->_owner() = 1;
        map["Carraia"].lock()->_owner() = 0;
        map["Barcellona"].lock()->_owner() = 2;

        builder1->start()
                ->move(4, 5, 9, 1, false)
                ->color(18, 1)
                ->house_count(18, 4)
                ->new_glitch("Testing new Glitch", "Dummy title", {"test", "foo", "bar"})
                ->other("bool", true)
                ->other("unsigned int", ((uint64_t) 19))
                ->other("signed int", ((int64_t) -19))
                ->other("double", 2.49)
                ->other("string", "foobar");
    }

    /*
     * Setting up dummy UpdateGameContainer.
     */
    Player p1{"player1", 2000, 0, nullptr}; //We don't need the quit function for this test
    Player p2{"player2", 2000, 0, nullptr};
    Player p3{"player3", 2000, 0, nullptr};

    shared_ptr<Player> sp1 = make_shared<Player> (p1);
    shared_ptr<Player> sp2 = make_shared<Player> (p2);
    shared_ptr<Player> sp3 = make_shared<Player> (p3);

    vector<weak_ptr<Player>> players{{sp1}, {sp2}, {sp3}};
    bool hasStarted = true;
    uint32_t turns = 29;
    quarantine_game::Map map = MapFactory::from_name("default-map");

    UpdateGameContainer container{players, &turns, &hasStarted, &map};

    GameUpdateBuilder *builder1 = new GameUpdateBuilder(container);
};

TEST_F(game_upd_bld_suite, test_result_json) {
    ASSERT_EQ(builder1->res().dump(), correct_json_game_update);

    delete builder1;
}