#ifndef CHESSDATA_H
#define CHESSDATA_H

#include <string>
#include "ChessLinkedListMoves.h"

class ChessData {
  public:
    explicit ChessData();

    void readSimpleGames(const std::string &filename);
    ChessLinkedListMoves *getMoves() const;

  private:
      std::unique_ptr<ChessLinkedListMoves> movesLinkedList;
};

#endif
