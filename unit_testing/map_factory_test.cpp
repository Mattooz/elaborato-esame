#include <gtest/gtest.h>
#include "../map_factory.h"

/*
 * Also test map_list() method inside the Map_Factory class.
 */
TEST(map_factory, test_from_name) {
    quarantine_game::Map test = quarantine_game::MapFactory::from_name("default-Map");

    ASSERT_EQ(test._map_name(), "default-Map");
    ASSERT_EQ(test._map_id(), "wWawlqCiQS61gCiy");

    ASSERT_EQ(test["Carraia"].lock()->_cost(), 260);
    ASSERT_EQ(dynamic_pointer_cast<quarantine_game::property_box>(test[19].lock())->_name(), "Agliana");
}

TEST(map_factory, test_from_id) {
    quarantine_game::Map test = quarantine_game::MapFactory::from_id("wWawlqCiQS61gCiy");

    ASSERT_EQ(test._map_name(), "default-Map");
    ASSERT_EQ(test._map_id(), "wWawlqCiQS61gCiy");

    ASSERT_EQ(test["Carraia"].lock()->_cost(), 260);
    ASSERT_EQ(dynamic_pointer_cast<quarantine_game::property_box>(test[19].lock())->_name(), "Agliana");
}

