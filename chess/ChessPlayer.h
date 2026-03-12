#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

class ChessInterface;

class ChessPlayer {
  public:
    explicit ChessPlayer(ChessInterface &chessInterface);
    virtual ~ChessPlayer() = default;

    virtual std::optional<std::string> getNextMove() = 0;

  protected:
    ChessInterface &_chessInterface;

  private:
};

using PlayerFactory = std::function<std::unique_ptr<ChessPlayer>(ChessInterface &)>;
