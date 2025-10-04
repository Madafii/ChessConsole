add_library(chessApi ChessWebInterface.h ChessWebInterface.cpp)

target_include_directories(chessApi PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chessApi chessBase chessML chessAnalyzer chess
                      httplib::httplib)
