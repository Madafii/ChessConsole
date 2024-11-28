#include "ChessMove.h"
#include <memory>

ChessMove::ChessMove(const std::string &board, const std::string &pgn, const std::string &move,
                     bool white, const RESULT &result)
    : board{board}, pgnName(pgn), moveName(move), white(white) {
    addResult(result);
}

ChessMove::~ChessMove() {}

ChessMove *ChessMove::addNext(const std::string &board, const std::string &pgn,
                              const std::string &move, const RESULT &result, const bool white) {
    ChessMove *next;
    const std::string &key = getKey();
    if (nexts.contains(key)) {
        next = nexts.at(key).get();
        next->addResult(result);
    } else {
        auto newMove = std::make_shared<ChessMove>(board, pgn, move, white, result);
        nexts[ChessMove::createKey(white, board)] = std::move(newMove);
        next = newMove.get();
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

std::string ChessMove::getKey() const { return ChessMove::createKey(white, board); }
