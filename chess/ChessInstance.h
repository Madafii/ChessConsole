//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSINSTANCE_H
#define CHESSINSTANCE_H
#include "ChessBoard.h"


class ChessInstance {
public:
    explicit ChessInstance();
    void run();
private:
    ChessBoard _chessBoard;
};



#endif //CHESSINSTANCE_H
