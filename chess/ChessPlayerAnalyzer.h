#pragma once

#include "ChessInterface.h"
#include "ChessPlayer.h"
#include "ChessAnalyzer.h"

class ChessPlayerAnalyzer : public ChessPlayer {
  public:
    explicit ChessPlayerAnalyzer(const ChessInterface &chessInterface);
    ~ChessPlayerAnalyzer() override;

    std::optional<std::string> getNextMove() override;

  private:
    ChessAnalyzer _chessAnalyzer;
};
