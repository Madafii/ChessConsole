#pragma once

#include "ChessInterface.h"
#include "ChessPlayer.h"
#include <memory>

enum class PlayerType { ConsoleHuman };

class ChessUI {
  public:
    ChessUI(PlayerType playerWhite, PlayerType playerBlack);

    virtual void start() = 0;

  protected:
    ChessInterface _chessInterface;
    std::unique_ptr<ChessPlayer> _playerWhite;
    std::unique_ptr<ChessPlayer> _playerBlack;

  private:
};
