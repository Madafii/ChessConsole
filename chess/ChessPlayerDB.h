#pragma once

#include "ChessDatabaseInterface.h"
#include "ChessInterface.h"
#include "ChessPlayer.h"
#include "ChessPlayerRnd.h"

#include <memory>
#include <ranges>
#include <string_view>

class ChessPlayerDB : public ChessPlayer {
  public:
    explicit ChessPlayerDB(const ChessInterface &chessInterface);
    explicit ChessPlayerDB(const ChessInterface &chessInterface, const std::string &dbName, const PlayerFactory &fallbackPlayer);

    std::optional<std::string> getNextMove() override;

  private:
    // database
    std::string _dbName = "chessMoves";
    ChessDatabaseInterface _chessDB = ChessDatabaseInterface(_dbName);
    table_pair _gameDepth = {0, false};
    int _fromMoveId = 1;
    // fallback
    std::unique_ptr<ChessPlayer> _fallbackPlayer = std::make_unique<ChessPlayerRnd>(_chessInterface);
    bool _useFallbackPlayer = false;

    bool updateOponentMoveId(std::string_view input);
    static uint64_t playedMoves(const MoveCompressed *move) { return move->wins + move->loses + move->draws; }

    const MoveCompressed *getMostPlayedMove(std::ranges::input_range auto &&moves) const;
};
