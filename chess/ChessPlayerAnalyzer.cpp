#include "ChessPlayerAnalyzer.h"
#include <optional>

ChessPlayerAnalyzer::ChessPlayerAnalyzer(const ChessInterface &chessInterface)
    : ChessPlayer(chessInterface), _chessAnalyzer(chessInterface.getChessBoard()) {}

ChessPlayerAnalyzer::~ChessPlayerAnalyzer() = default;

std::optional<std::string> ChessPlayerAnalyzer::getNextMove() {
    const auto bestMoves = _chessAnalyzer.getBestEvalMoves(1);
    if (bestMoves.empty()) return std::nullopt;
    return bestMoves.front().second;
}
