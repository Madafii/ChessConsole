#pragma once

#include "ChessInterface.h"
#include "ChessPlayer.h"
#include <memory>

using PlayerFactory = std::function<std::unique_ptr<ChessPlayer>(const ChessInterface &)>;

class ChessUI {
  public:
    ChessUI(PlayerFactory playerWhite, PlayerFactory playerBlack);

    virtual void start() = 0;

  protected:
    ChessInterface _chessInterface;
    std::unique_ptr<ChessPlayer> _playerWhite;
    std::unique_ptr<ChessPlayer> _playerBlack;

  private:
};
