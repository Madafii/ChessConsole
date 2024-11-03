//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSTILE_H
#define CHESSTILE_H
#include <memory>
#include <unordered_map>

#include "ChessPiece.h"


class ChessTile {
public:
    explicit ChessTile(std::unique_ptr<ChessPiece> piece, int x, int y);

    int getX() const {return _x;}
    int getY() const {return _y;}
    std::string getMove() const;
    std::unique_ptr<ChessPiece> piece;

    static const std::unordered_map<char, int> mapXtoInt;
    static const std::unordered_map<int, char> mapIntToX;

private:
    int _x;
    int _y;
};

#endif //CHESSTILE_H
