cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)
project(chess_control)

set(CMAKE_CXX_STANDARD 26)

add_executable(chess_control main.cpp)

add_subdirectory(chess)
add_subdirectory(tests)

target_link_libraries(chess_control PRIVATE chess)