#ifndef CHESSTILE_H
#define CHESSTILE_H

#include "ChessPiece.h"

#include <unordered_map>

class ChessTile {
  public:
    explicit ChessTile(const ChessPiece &piece, int x, int y);

    [[nodiscard]] int getX() const { return _x; }
    [[nodiscard]] int getY() const { return _y; }
    [[nodiscard]] std::string getMove() const { return std::string(mapIntToX.at(_x) + std::to_string(_y + 1)); }

    [[nodiscard]] ChessPiece &getPiece() { return _piece; }
    [[nodiscard]] ChessPiece getPiece() const { return _piece; }
    void changePiece(const ChessPiece &piece) { _piece = piece; }
    void switchPiece(ChessPiece &piece) { std::swap(_piece, piece); }

    // default checks if any piece on tile
    bool hasPiece(const ChessPieceType pType = ChessPieceType::NONE) const { return _piece.getType() == pType; }
    bool hasWhitePiece() const { return _piece.isWhite(); }

    static const std::unordered_map<char, int> mapXtoInt;
    static const std::unordered_map<int, char> mapIntToX;

  private:
    ChessPiece _piece;
    int _x;
    int _y;
};

#endif // CHESSTILE_H
