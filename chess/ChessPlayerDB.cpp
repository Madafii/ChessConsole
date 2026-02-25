#include "ChessPlayerDB.h"
#include "ChessBoard.h"
#include "ChessLinkedListMoves.h"
#include "ChessUI.h"
#include <iostream>

ChessPlayerDB::ChessPlayerDB(const ChessInterface &chessInterface) : ChessPlayer(chessInterface) {}

ChessPlayerDB::ChessPlayerDB(const ChessInterface &chessInterface, const std::string &dbName, const PlayerFactory &fallbackPlayer)
    : ChessPlayer(chessInterface), _dbName(dbName), _chessDB(dbName), _fallbackPlayer(fallbackPlayer(_chessInterface)) {}

std::optional<std::string> ChessPlayerDB::getNextMove() {
    if (_useFallbackPlayer) return _fallbackPlayer->getNextMove();
    const std::string input = _chessInterface.getChessBoard().getLastMove();

    // input could be empty if the database makes the first move
    if (!input.empty()) {
        if (!updateOponentMoveId(input)) {
            std::cout << "using fallback player from now on" << std::endl;
            _useFallbackPlayer = true;
            return _fallbackPlayer->getNextMove();
        }
    }
    auto nextMoves = _chessDB.getNextMoves(_gameDepth, _fromMoveId);

    // get the move string
    auto bestMove = getMostPlayedMove(nextMoves | std::views::elements<1>);
    std::string dbMove = ChessLinkedListMoves::getMoveFromData(bestMove->data);

    // increment own move
    _fromMoveId = *_chessDB.getMoveIdOpt(_gameDepth, _fromMoveId, bestMove->data);
    ++_gameDepth;

    return dbMove;
}

bool ChessPlayerDB::updateOponentMoveId(std::string_view input) {
    const auto moveData = ChessLinkedListMoves::createData(input, _chessInterface.getChessBoard().isWhitesTurn());
    auto oponentMoveId = _chessDB.getMoveIdOpt(_gameDepth, _fromMoveId, moveData);
    if (!oponentMoveId) return false;

    // increment oponents move
    _fromMoveId = *oponentMoveId;
    ++_gameDepth;
    return true;
}

const MoveCompressed *ChessPlayerDB::getMostPlayedMove(std::ranges::input_range auto &&moves) const {
    const MoveCompressed *mostPlayedMove = nullptr;
    uint64_t currPlays = 0;
    for (const MoveCompressed &move : moves) {
        const uint64_t plays = playedMoves(&move);
        if (mostPlayedMove == nullptr || plays > currPlays) {
            mostPlayedMove = &move;
            currPlays = plays;
        }
    }
    return mostPlayedMove;
}
