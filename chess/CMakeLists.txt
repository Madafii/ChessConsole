add_library(chess
    ChessPiece.cpp ChessPiece.h
    ChessTile.cpp ChessTile.h
    ChessBoard.cpp ChessBoard.h
    ChessInstance.cpp ChessInstance.h
)

target_include_directories(chess PUBLIC ".")
target_link_libraries(chess)