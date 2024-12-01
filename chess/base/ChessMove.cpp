#include "ChessMove.h"

#include <fcntl.h>
#include <memory>

ChessMove::ChessMove(const std::string &board, const std::string &pgn, const std::string &move,
                     bool white, const RESULT &result)
    : board{board}, pgnName(pgn), moveName(move), white(white), wins{0}, loses{0}, draws{0} {
    addResult(result);
}

ChessMove::~ChessMove() {}

ChessMove *ChessMove::addNext(const std::string &nextBoard, const std::string &nextPGN,
                              const std::string &nextMove, const RESULT &result, const bool nextWhite) {
    ChessMove *next;
    const std::string &key = createKey(nextWhite, nextBoard);
    if (nexts.contains(key)) {
        next = nexts.at(key).get();
        next->addResult(result);
    } else {
        auto newMove = std::make_shared<ChessMove>(nextBoard, nextPGN, nextMove, nextWhite, result);
        next = newMove.get();
        nexts[createKey(nextWhite, nextBoard)] = std::move(newMove);
    }
    return next;
}

void ChessMove::addResult(const RESULT &result) {
    switch (result) {
    case RESULT::WIN:
        wins++;
        break;
    case RESULT::LOSE:
        loses++;
        break;
    case RESULT::DRAW:
        draws++;
        break;
    default:
        break;
    }
}

std::string ChessMove::getKey() const { return createKey(white, board); }
