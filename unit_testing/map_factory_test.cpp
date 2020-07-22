#include <gtest/gtest.h>
#include "../map_factory.h"

/*
 * Also test map_list() method inside the map_factory class.
 */
TEST(map_factory, test_from_name) {
    quarantine_game::map test = quarantine_game::map_factory::from_name("default-map");

    ASSERT_EQ(test._map_name(), "default-map");
    ASSERT_EQ(test._map_id(), "wWawlqCiQS61gCiy");

    ASSERT_EQ(test["Carraia"].lock()->_cost(), 260);
    ASSERT_EQ(dynamic_pointer_cast<quarantine_game::property_box>(test[19].lock())->_name(), "Agliana");
}

TEST(map_factory, test_from_id) {
    quarantine_game::map test = quarantine_game::map_factory::from_id("wWawlqCiQS61gCiy");

    ASSERT_EQ(test._map_name(), "default-map");
    ASSERT_EQ(test._map_id(), "wWawlqCiQS61gCiy");

    ASSERT_EQ(test["Carraia"].lock()->_cost(), 260);
    ASSERT_EQ(dynamic_pointer_cast<quarantine_game::property_box>(test[19].lock())->_name(), "Agliana");
}

