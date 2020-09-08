#include <gtest/gtest.h>
#include "../map_factory.h"

/*
 * Also test map_list() method inside the Map_Factory class.
 */
TEST(map_factory, test_from_name_valid) {
    QuarantineGame::Map test = QuarantineGame::MapFactory::from_name("default-map");

    ASSERT_EQ(test._map_name(), "default-map");
    ASSERT_EQ(test._map_id(), "wWawlqCiQS61gCiy");

    ASSERT_EQ(test["Carraia"]->_cost(), 260);
    ASSERT_EQ(dynamic_pointer_cast<QuarantineGame::PropertyBox>(test[19])->_name(), "Agliana");
}

TEST(map_factory, test_from_id_valid) {
    QuarantineGame::Map test = QuarantineGame::MapFactory::from_id("wWawlqCiQS61gCiy");

    ASSERT_EQ(test._map_name(), "default-map");
    ASSERT_EQ(test._map_id(), "wWawlqCiQS61gCiy");

    ASSERT_EQ(test["Carraia"]->_cost(), 260);
    ASSERT_EQ(dynamic_pointer_cast<QuarantineGame::PropertyBox>(test[19])->_name(), "Agliana");
}

TEST(map_factory, test_from_name_not_found) {
    QuarantineGame::Map test = QuarantineGame::MapFactory::from_name("not-a-map");

    ASSERT_EQ(test._map_name(), "not found");
    ASSERT_EQ(test._map_id(), "not found");
}

TEST(map_factory, test_from_id_not_found) {
    QuarantineGame::Map test = QuarantineGame::MapFactory::from_id("not-a-map");

    ASSERT_EQ(test._map_name(), "not found");
    ASSERT_EQ(test._map_id(), "not found");
}

