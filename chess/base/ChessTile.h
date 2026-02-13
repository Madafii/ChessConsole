#ifndef CHESSTILE_H
#define CHESSTILE_H

#include "ChessPiece.h"

class ChessTile {
  public:
    ChessTile() = default;
    explicit ChessTile(const ChessPiece &piece, int x, int y);

    [[nodiscard]] int getX() const { return _x; }
    [[nodiscard]] int getY() const { return _y; }

    [[nodiscard]] std::string getPos() const { return std::string(static_cast<char>('a' + _x) + std::to_string(_y + 1)); }
    [[nodiscard]] static std::string getPos(int x, int y) { return std::string(static_cast<char>('a' + x) + std::to_string(y + 1)); }
    [[nodiscard]] static std::pair<int, int> getPos(char x, char y) { return {x - 'a', y - '0'}; }

    [[nodiscard]] ChessPiece &getPiece() { return _piece; }
    [[nodiscard]] const ChessPiece &getPiece() const { return _piece; }

    [[nodiscard]] bool isEmpty() const { return _piece.getType() == ChessPieceType::NONE; }

    [[nodiscard]] ChessPieceType getPieceType() const { return _piece.getType(); }

    void changePiece(const ChessPiece &piece) { _piece = piece; }
    void switchPiece(ChessPiece &piece) { std::swap(_piece, piece); }
    void occupyPiece(ChessPiece &piece) {
        piece = _piece;
        _piece = ChessPiece();
    }

    // default checks if any piece on tile
    bool hasPiece(const ChessPieceType pType) const { return _piece.getType() == pType; }
    bool hasWhitePiece() const { return _piece.isWhite(); }
    bool hasBlackPiece() const { return !_piece.isWhite(); }

    bool operator<(const ChessTile &other) const { return std::pair(_x, _y) < std::pair(other._x, other._y); }
    bool operator==(const ChessTile &other) const { return _x == other._x && _y == other._y; }

  private:
    ChessPiece _piece;
    int _x = 0;
    int _y = 0;
};

#endif // CHESSTILE_H
