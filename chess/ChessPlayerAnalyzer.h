#pragma once

#include "ChessAnalyzer.h"
#include "ChessInterface.h"
#include "ChessPlayer.h"

class ChessPlayerAnalyzer : public ChessPlayer {
  public:
    explicit ChessPlayerAnalyzer(ChessInterface &chessInterface);
    ~ChessPlayerAnalyzer() override;

    std::optional<std::string> getNextMove() override;

  private:
    ChessAnalyzer _chessAnalyzer;
};
