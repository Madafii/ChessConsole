//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSINSTANCE_H
#define CHESSINSTANCE_H
#include "ChessBoard.h"

class ChessInstance {
  public:
    explicit ChessInstance();
    ~ChessInstance();

    void run();
    void runRandom();
    void runAgainstRandom(bool white);
    void runWithChessData();

  private:
    ChessBoard *chessBoard;
};

#endif // CHESSINSTANCE_H
