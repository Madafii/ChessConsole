#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

class ChessInterface;

class ChessPlayer {
  public:
    explicit ChessPlayer(const ChessInterface &chessInterface);
    virtual ~ChessPlayer() = default;

    virtual std::optional<std::string> getNextMove() = 0;

  protected:
    const ChessInterface &_chessInterface;

  private:
};

using PlayerFactory = std::function<std::unique_ptr<ChessPlayer>(const ChessInterface &)>;
