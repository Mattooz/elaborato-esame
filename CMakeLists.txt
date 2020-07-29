cmake_minimum_required(VERSION 3.15)
project(elaborato_esame)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(unit_testing)
add_library(core shortid.h shortid.cpp player.cpp player.h update_builder.h update_builder.cpp map.cpp map.h utils.cpp utils.h map_factory.cpp map_factory.h glitch.cpp glitch.h glitch_handler.cpp glitch_handler.h game.cpp game.h)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(core PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(core PUBLIC spdlog::spdlog)

include_directories(/Users/niccolomattei/Simple-Web-Server)

set(BOOST_ROOT "/Users/niccolomattei/Boost/boost_1_73_0")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.73.0 COMPONENTS)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(core ${Boost_LIBRARIES})
endif()

add_executable(elaborato_esame main.cpp)
target_link_libraries(elaborato_esame core)

