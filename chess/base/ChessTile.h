#ifndef CHESSTILE_H
#define CHESSTILE_H

#include "ChessPiece.h"

#include <unordered_map>

class ChessTile {
  public:
    explicit ChessTile(const ChessPiece &piece, int x, int y);

    [[nodiscard]] inline int getX() const { return _x; }
    [[nodiscard]] inline int getY() const { return _y; }
    [[nodiscard]] inline ChessPiece getPiece() const { return _piece; }
    [[nodiscard]] inline std::string getMove() const { return std::string(mapIntToX.at(_x) + std::to_string(_y + 1)); }

    inline void changePiece(const ChessPiece &piece) { _piece = piece; }

    static const std::unordered_map<char, int> mapXtoInt;
    static const std::unordered_map<int, char> mapIntToX;

  private:
    ChessPiece _piece;
    int _x;
    int _y;
};

#endif // CHESSTILE_H
