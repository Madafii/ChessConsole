add_executable(chess-web main.cpp)

add_subdirectory(api)
add_subdirectory(libs/cpp-httplib httplib)

target_link_libraries(chess-web PUBLIC chessApi)
