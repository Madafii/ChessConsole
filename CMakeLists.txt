cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)
project(chess_control CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic")

include_directories(${CMAKE_SOURCE_DIR}/chess/base)

add_executable(chess_control main.cpp)

add_subdirectory(chess)
add_subdirectory(web)
add_subdirectory(scripts)
add_subdirectory(tests)
add_subdirectory(libs/libpqxx build-pqxx)
add_subdirectory(libs/spdlog spdlog)
add_subdirectory(libs/json json)

target_link_libraries(chess_control PRIVATE chess)
