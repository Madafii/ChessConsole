cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(chess_control)

add_subdirectory(chess)

add_executable(chess_control main.cpp)
target_link_libraries(chess_control PRIVATE chess)