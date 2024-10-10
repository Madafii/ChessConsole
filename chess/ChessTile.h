//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSTILE_H
#define CHESSTILE_H
#include "ChessPiece.h"


class ChessTile {
public:
    explicit ChessTile(ChessPiece *piece, int x, int y);

    int getX() const {return _x;}
    int getY() const {return _y;}
    ChessPiece* piece;
private:
    int _x;
    int _y;
};



#endif //CHESSTILE_H
