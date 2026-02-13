#pragma once

#include <optional>
#include <string>

class ChessInterface;

class ChessPlayer {
  public:
    explicit ChessPlayer(const ChessInterface &chessInterface);
    virtual ~ChessPlayer() = default;

    virtual std::optional<std::string> getNextMove() const = 0;

  protected:
    const ChessInterface &_chessInterface;

  private:
};
