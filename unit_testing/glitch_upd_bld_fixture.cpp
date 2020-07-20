#include <gtest/gtest.h>
#include "../update_builder.h"
#include <iostream>
#include "../utils.h"

using namespace quarantine_game;

/*
 * Correct json output generated with the Java version of the builder.
 */
const string correct_json_glitch_update = utils::read_file(
        "/Users/niccolomattei/CLionProjects/elaborato-esame/unit_testing/glitch_builder_json_output.json");

class glitch_upd_bld_suite : public ::testing::Test {
protected:
    virtual void SetUp() {
        builder1->glitch_error("test error 1", 1)
                ->glitch_error("test error 2", 2)
                ->glitch_warning("test warning 1", 1)
                ->glitch_warning("test warning 2", 2)
                ->glitch_success("test success 1", 1)
                ->glitch_success("test success 2", 2);
    }

    glitch_update_builder *builder1 = new glitch_update_builder();
};

TEST_F(glitch_upd_bld_suite, test_result_json) {
    ASSERT_EQ(builder1->res().dump(), correct_json_glitch_update);
}
