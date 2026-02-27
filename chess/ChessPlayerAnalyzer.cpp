#include "ChessPlayerAnalyzer.h"
#include <optional>

ChessPlayerAnalyzer::ChessPlayerAnalyzer(const ChessInterface &chessInterface)
    : ChessPlayer(chessInterface), _chessAnalyzer(chessInterface.getChessBoard()) {}

ChessPlayerAnalyzer::~ChessPlayerAnalyzer() = default;

std::optional<std::string> ChessPlayerAnalyzer::getNextMove() {
    return _chessAnalyzer.getBestEvalMove(0);
}
