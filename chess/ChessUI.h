#pragma once

#include "ChessInterface.h"
#include "ChessPlayer.h"
#include <memory>

class ChessUI {
  public:
    ChessUI(const PlayerFactory &playerWhite, const PlayerFactory &playerBlack);

    virtual void start() = 0;

  protected:
    ChessInterface _chessInterface;
    std::unique_ptr<ChessPlayer> _playerWhite;
    std::unique_ptr<ChessPlayer> _playerBlack;

  private:
};
