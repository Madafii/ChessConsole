#pragma once

#include <random>

#include "ChessInterface.h"
#include "ChessPlayer.h"

class ChessPlayerRnd : public ChessPlayer {
  public:
    ChessPlayerRnd(const ChessInterface &chessInterface);
    ~ChessPlayerRnd() override;

    std::optional<std::string> getNextMove() const override;

  private:
    mutable std::mt19937 _gen;
    static constexpr std::array<std::string_view, 4> promo = {"=q", "=r", "=b", "=n"};

    std::string_view getRndPawnPromotion() const;
};
