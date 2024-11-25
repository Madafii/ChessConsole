#include "ChessMove.h"

ChessMove::ChessMove(const std::string &board, const std::string &pgn, const std::string &move, bool white)
    : board{board}, pgnName(pgn), moveName(move), white(white), wins(0), loses(0), draws(0) {
}

ChessMove::~ChessMove() {}

void ChessMove::addNext(const std::string &board, const std::string &pgn, const std::string &move, const Result &result,
                        const bool white) {
    if (nexts.contains(getKey())) {
        switch (result) {
            case Result::WIN:
                wins++;
                break;
            case Result::LOSE:
                loses++;
                break;
            case Result::DRAW:
                draws++;
                break;
        }
    } else {
        nexts[(white ? "W|" : "B|") + board] = new ChessMove(board, pgn, move, white);
    }
}

std::string ChessMove::getKey() const {
    return (white ? "W|" : "B|") + board;
}
