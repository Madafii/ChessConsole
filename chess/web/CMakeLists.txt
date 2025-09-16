add_library(chessWeb ChessWebInterface.h ChessWebInterface.cpp)

target_include_directories(chessWeb PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chessWeb chessBase chessML httplib::httplib)
