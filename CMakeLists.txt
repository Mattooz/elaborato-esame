cmake_minimum_required(VERSION 3.15)
project(elaborato_esame)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(unit_testing)
add_library(core shortid.h shortid.cpp)

add_executable(elaborato_esame main.cpp)
target_link_libraries(elaborato_esame core)

