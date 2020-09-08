#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

int main(int argc, char ** argv) {
    spdlog::default_logger()->set_level(spdlog::level::off);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

