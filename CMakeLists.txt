cmake_minimum_required(VERSION 3.15)
project(elaborato_esame)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(unit_testing)
add_library(core shortid.h shortid.cpp player.cpp player.h update_builder.h update_builder.cpp map.cpp map.h utils.cpp utils.h map_factory.cpp map_factory.h)

find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(core PUBLIC nlohmann_json::nlohmann_json)

add_executable(elaborato_esame main.cpp)
target_link_libraries(elaborato_esame core)

