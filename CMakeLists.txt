cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)
project(chess_control CXX)

option(BUILD_WEB "Build for web deployment" ON)
option(BUILD_SCRIPTS "Build scripts" ON)
option(BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic")

add_compile_definitions(DATABASE)

include_directories(${CMAKE_SOURCE_DIR}/chess/base)

add_executable(chess_control main.cpp)

add_subdirectory(chess)
if(BUILD_WEB)
  add_subdirectory(web)
endif()
if(BUILD_SCRIPTS)
  add_subdirectory(scripts)
endif()
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

# libraries
add_subdirectory(libs/libpqxx build-pqxx)
# currently unused add_subdirectory(libs/spdlog spdlog)
# add_subdirectory(libs/json json)

target_link_libraries(chess_control PRIVATE chess)
