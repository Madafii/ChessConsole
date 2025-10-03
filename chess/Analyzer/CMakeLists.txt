add_library(chessAnalyzer ChessAnalyzer.h ChessAnalyzer.cpp)

target_include_directories(chessAnalyzer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chessAnalyzer chessBase)
