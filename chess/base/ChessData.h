#ifndef CHESSDATA_H
#define CHESSDATA_H

#include "ChessMove.h"
#include <memory>
#include <string>
#include <vector>

class ChessData {
  public:
    explicit ChessData();

    void readSimpleGames(const std::string &filename);

  private:
    // basically the root of ChessMove
    std::vector<std::unique_ptr<ChessMove>> startMoves;
};

#endif
