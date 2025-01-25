#include "ChessAnalyzer.h"
#include "ChessBoardDraw.h"
#include "ChessMoveLogic.h"
#include <optional>
#include <string>
#include <string_view>

using oStrVec = ChessAnalyzer::oStrVec;
using attackMatrix = ChessAnalyzer::attackMatrix;

ChessAnalyzer::ChessAnalyzer(ChessBoard &aboard) : origBoard(aboard) {}

/// returns a vector of moves if a forced checkmate is possible
oStrVec ChessAnalyzer::getForcedCheckmate(int depth) {
    const bool white = origBoard.isWhitesTurn();
    const Pieces allMoves = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();

    return std::nullopt;
}

oStrVec ChessAnalyzer::getFreePieces(const attackMatrix &matr, const bool white) {}
/// return a matrix with each tile and the corresponding attackers of that tile, where the first pair is the white attackers and the second
/// the black attackers
///
attackMatrix ChessAnalyzer::getAttackedMatrix() {
    attackMatrix attackedBy(64);
    addToAttackedMatrix(attackedBy, true);
    addToAttackedMatrix(attackedBy, false);
    return attackedBy;
}

double ChessAnalyzer::evalCurrPosition(bool white) {}

double ChessAnalyzer::evalPawnStruct(bool white) {}

void ChessAnalyzer::addToAttackedMatrix(attackMatrix &attackedBy, bool white) {
    const Pieces colorPieces = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();

    for (const auto attackingTile : colorPieces) {
        Pieces attackedTiles = ChessMoveLogic::getPossibleMoves(origBoard, attackingTile);
        ChessMoveLogic::filterPossibleMovesForChecks(origBoard, attackingTile, attackedTiles);
        for (const auto attackedTile : attackedTiles) {
            if (white) {
                attackedBy[attackedTile->getY() * boardWidth + attackedTile->getX()].first.push_back(attackingTile);
            } else {
                attackedBy[attackedTile->getY() * boardWidth + attackedTile->getX()].second.push_back(attackingTile);
            }
        }
    }
}
