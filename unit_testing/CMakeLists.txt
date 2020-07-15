cmake_minimum_required(VERSION 3.15)

set(GOOGLETEST_VERSION 1.10.0)

add_subdirectory(./lib/googletest)
set(gtest_SOURCE_DIR, ./lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TEST test.cpp shortid_test.cpp)
add_executable(test ${TEST})
target_link_libraries(test gtest gtest_main core)