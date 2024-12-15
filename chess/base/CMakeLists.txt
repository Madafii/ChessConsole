add_library(chessBase
        ChessPiece.cpp ChessPiece.h
        ChessTile.cpp ChessTile.h
        ChessBoard.cpp ChessBoard.h
        ChessUtils.cpp ChessUtils.h
        ChessBoardDraw.cpp ChessBoardDraw.h
)

target_include_directories(chessBase PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chessBase)
