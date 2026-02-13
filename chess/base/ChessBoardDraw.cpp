#include "ChessBoardDraw.h"
#include "ChessBoard.h"
#include "ChessPiece.h"
#include "ChessTile.h"

#include <algorithm>
#include <iostream>

ChessBoardDraw::ChessBoardDraw(const ChessBoardDrawSettings settings) : _settings(settings) {}

void ChessBoardDraw::draw(const ChessBoard &board, const PieceTiles &highlightTiles) const {
    if (_settings.sysClear) system("clear");
    // Print the chessboard with characters
    for (int y = 7; y >= 0; --y) {
        std::cout << " " << y + 1 << " ";
        for (int x = 0; x < 8; ++x) {
            const ChessPiece &drawPiece = board.getTileAt(x, y).getPiece();
            if ((y + x) % 2 == 0) {
                if (isHighlighted(x, y, highlightTiles)) {
                    std::cout << _backgroundColorHighlightWhite;
                } else {
                    // White square
                    std::cout << _backgroundColorWhite;
                }
            } else {
                if (isHighlighted(x, y, highlightTiles)) {
                    std::cout << _backgroundColorHighlightBlack;
                } else {
                    // Black square
                    std::cout << _backgroundColorBlack;
                }
            }
            if (drawPiece.isWhite()) {
                // green
                std::cout << _pieceColorWhite;
            } else {
                // magenta
                std::cout << _pieceColorBlack;
            }
            if (drawPiece.getType() == ChessPieceType::NONE) {
                std::cout << "   ";
            } else if (_settings.useNerdFont) {
                // use black pieces for both (still having different colors)
                // std::cout << " " << _blackNerdFontPieces.at(drawPiece.getShortName()) << " ";
                if (drawPiece.isWhite()) {
                    std::cout << " " << _whiteNerdFontPieces.at(drawPiece.getShortName()) << " ";
                } else {
                    std::cout << " " << _blackNerdFontPieces.at(drawPiece.getShortName()) << " ";
                }
            } else {
                std::cout << " " << drawPiece.getShortName() << " ";
            }
        }
        std::cout << _defaultColor << std::endl; // Reset to default color after each line
    }
    std::cout << "    a  b  c  d  e  f  g  h " << std::endl;
}

bool ChessBoardDraw::isHighlighted(int x, int y, const PieceTiles &highlightTiles) const {
    return std::ranges::any_of(highlightTiles, [x, y](const ChessTile *tile) { return tile->getX() == x && tile->getY() == y; });
}
