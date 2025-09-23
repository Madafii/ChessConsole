add_library(chessWeb ChessWebInterface.h ChessWebInterface.cpp)

target_include_directories(chessWeb PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chessWeb chess httplib::httplib
                      nlohmann_json::nlohmann_json)
