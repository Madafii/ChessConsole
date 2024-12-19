#ifndef CHESSTILE_H
#define CHESSTILE_H

#include "ChessPiece.h"

#include <memory>
#include <unordered_map>

class ChessTile {
  public:
    explicit ChessTile(std::unique_ptr<ChessPiece> piece, int x, int y);

    [[nodiscard]] int getX() const { return _x; }
    [[nodiscard]] int getY() const { return _y; }
    [[nodiscard]] std::string getMove() const;
    std::unique_ptr<ChessPiece> piece;

    static const std::unordered_map<char, int> mapXtoInt;
    static const std::unordered_map<int, char> mapIntToX;

  private:
    int _x;
    int _y;
};

#endif // CHESSTILE_H
