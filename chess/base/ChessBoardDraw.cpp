#include "ChessBoardDraw.h"

#include <iostream>

ChessBoardDraw::ChessBoardDraw(const ChessBoardDrawSettings settings) : settings(settings) {}

void ChessBoardDraw::draw(const ChessBoard &board) const {
    if (settings.sysClear) system("clear");
    // Print the chessboard with characters
    for (int y = 7; y >= 0; --y) {
        std::cout << " " << y + 1 << " ";
        for (int x = 0; x < 8; ++x) {
            if ((y + x) % 2 == 0) {
                // Black square
                std::cout << "\x1b[48;5;0m";
            } else {
                // White square
                std::cout << "\x1b[48;5;231m";
            }
            if (board.getTileAt(x, y).getPiece().getType() == ChessPieceType::NONE) {
                std::cout << "   ";
                continue;
            }
            if (board.getTileAt(x, y).getPiece().isWhite()) {
                // green
                std::cout << "\x1b[38;5;22m";
            } else {
                // magenta
                std::cout << "\x1b[38;5;126m";
            }
            std::cout << " " << board.getTileAt(x, y).getPiece().getShortName() << " ";
        }
        std::cout << "\x1b[0m" << std::endl; // Reset to default color after each line
    }
    std::cout << "    a  b  c  d  e  f  g  h " << std::endl;
}

inline ChessBoardDrawSettings &ChessBoardDraw::getSettings() { return settings; }
