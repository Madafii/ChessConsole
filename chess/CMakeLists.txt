add_library(chess ChessInstance.h ChessInstance.cpp)

target_include_directories(chess PUBLIC ${CMAKE_CURRENT_LIST_DIR})
# target_link_libraries(chess chessBase chessML chessAnalyzer) deactivated
target_link_libraries(chess chessBase chessML)

add_subdirectory(base)
add_subdirectory(ML)
# add_subdirectory(Analyzer)
