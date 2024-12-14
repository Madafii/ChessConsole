add_library(chessML
        ChessData.h ChessData.cpp
        ChessLinkedListMoves.h ChessLinkedListMoves.cpp
)

target_include_directories(chessML PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chessML chessBase)
