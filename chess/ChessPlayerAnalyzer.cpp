#include "ChessPlayerAnalyzer.h"
#include <optional>

ChessPlayerAnalyzer::ChessPlayerAnalyzer(ChessInterface &chessInterface)
    : ChessPlayer(chessInterface), _chessAnalyzer(chessInterface.getChessBoard()) {}

ChessPlayerAnalyzer::~ChessPlayerAnalyzer() = default;

std::optional<std::string> ChessPlayerAnalyzer::getNextMove() {
    auto moves = _chessAnalyzer.getEvalTree(2);
    return _chessAnalyzer.getBestEvaluatedMove(moves)->s_move;
}
