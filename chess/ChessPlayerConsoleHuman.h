#pragma once

#include "ChessBoardDraw.h"
#include "ChessInterface.h"
#include "ChessPlayer.h"

class ChessPlayerConsoleHuman : public ChessPlayer {
  public:
    explicit ChessPlayerConsoleHuman(const ChessInterface &chessInterface,
                                     const ChessBoardDrawSettings &settings = ChessBoardDrawSettings());
    ~ChessPlayerConsoleHuman() override;

    std::optional<std::string> getNextMove() override;

  private:
    ChessBoardDraw _boardDraw;

    std::optional<std::string> promptInput(const std::string &message) const;
    std::optional<PieceTiles> getValidatedMoves(std::string_view input) const;
};
