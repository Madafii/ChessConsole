add_library(chessBase
        ChessPiece.cpp ChessPiece.h
        ChessTile.cpp ChessTile.h
        ChessBoard.cpp ChessBoard.h
        ChessInstance.cpp ChessInstance.h
        ChessUtils.cpp ChessUtils.h
)

target_include_directories(chessBase PRIVATE ".")
target_link_libraries(chessBase)
