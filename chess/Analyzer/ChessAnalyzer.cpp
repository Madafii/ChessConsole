#include "ChessAnalyzer.h"
#include "ChessBoardDraw.h"
#include "ChessMoveLogic.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

using oStrVec = ChessAnalyzer::oStrVec;
using boardMatrix = ChessAnalyzer::boardMatrix;

ChessAnalyzer::ChessAnalyzer(ChessBoard &aboard) : origBoard(aboard) {}

// TODO: implement terminal control of analyzer
std::string ChessAnalyzer::startTerminalAnalyzer() {
    std::string input;
    std::cin >> input;

    auto getAttackerBoard = getAttackedMatrix();
    auto getDefenderBoard = getDefendedMatrix();
    std::cout << std::format("attacking total: {}\ndefending total: {}",
                             ChessAnalyzer::boardMatrixSize(getAttackerBoard, origBoard.isWhitesTurn()),
                             ChessAnalyzer::boardMatrixSize(getDefenderBoard, origBoard.isWhitesTurn()))
              << std::endl;
    auto freePieces = getFreePieces(getAttackerBoard, getDefenderBoard, origBoard.isWhitesTurn());
    for (const auto *piece : freePieces) {
        std::cout << std::format("can take: {} at x:{} y:{}", piece->piece->getFullName(), piece->getX() + 1, piece->getY() + 1)
                  << std::endl;
    }
}

/// returns a vector of moves if a forced checkmate is possible
oStrVec ChessAnalyzer::getForcedCheckmate(int depth) {
    const bool white = origBoard.isWhitesTurn();
    const Pieces allMoves = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();

    return std::nullopt;
}

std::vector<std::string> ChessAnalyzer::getBestEvalMoves() {
    const bool white = origBoard.isWhitesTurn();
    const Pieces allPossMoves = ChessMoveLogic::getAllPossibleMoves(origBoard, white);

    std::vector<std::pair<double, std::string>> evalMovesList(allPossMoves.size());
    for (const ChessTile *move : allPossMoves) {
        // make a copy of the board play the move and evaluate the position
        ChessBoard simulateBoard(origBoard);
        std::string moveName = move->getMove();
        simulateBoard.handleMoveInput(moveName);
        ChessAnalyzer boardAnalyzer(simulateBoard);
        const double evalValue = boardAnalyzer.evalCurrPosition(white);
        evalMovesList.emplace_back(evalValue, std::move(moveName));
    }

    // sort which position has the best evaluation
    std::sort(evalMovesList.begin(), evalMovesList.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
    std::vector<std::string> moves(evalMovesList.size());
    for (auto &valuePair : evalMovesList) {
        moves.push_back(std::move(valuePair.second));
    }
    return moves;
}

Pieces ChessAnalyzer::getFreePieces(const boardMatrix &attackMatr, const boardMatrix &defendMatr, const bool white) {
    Pieces freePieces;
    const Pieces attackedPieces = getCoveredPieces(attackMatr, white);
    const Pieces defendedPieces = getCoveredPieces(defendMatr, !white);
    // TODO: there is probably better algorithm for finding unique eles
    for (ChessTile *piece : attackedPieces) {
        auto found = std::find(defendedPieces.begin(), defendedPieces.end(), piece);
        if (found != defendedPieces.end()) {
            freePieces.push_back(piece);
        }
    }
    return freePieces;
}

/// return a matrix with each tile and the corresponding attackers of that tile, where the first pair is the white attackers and the second
/// the black attackers
///
boardMatrix ChessAnalyzer::getAttackedMatrix() {
    boardMatrix attackedBy(64);
    addToAttackedMatrix(attackedBy, true);
    addToAttackedMatrix(attackedBy, false);
    return attackedBy;
}

boardMatrix ChessAnalyzer::getDefendedMatrix() {
    boardMatrix defendedBy(64);
    addToDefendMatrix(defendedBy, true);
    addToDefendMatrix(defendedBy, false);
    return defendedBy;
}

size_t ChessAnalyzer::boardMatrixSize(const boardMatrix &boardMatr, const bool white) {
    size_t count = 0;
    for (const auto &[whiteTiles, blackTiles] : boardMatr) {
        count += white ? whiteTiles.size() : blackTiles.size();
    }
    return count;
}

// returns the attacked pieces of the opponent
Pieces ChessAnalyzer::getCoveredPieces(const boardMatrix &boardMat, const bool white) {
    Pieces converedPieces;
    for (int i = 0; i < boardMat.size(); i++) {
        const Pieces selTiles = white ? boardMat[i].first : boardMat[i].second;
        if (selTiles.empty()) continue; // not attacked
        ChessTile *coveredTile = origBoard.getTileAt(i);
        if (coveredTile->piece == nullptr) continue; // attacks empty tile
        converedPieces.push_back(coveredTile);
    }
    return converedPieces;
}

Pieces ChessAnalyzer::getCoveredTiles(const boardMatrix &boardMat, const bool white) {}

int ChessAnalyzer::getPieceValue(bool white) {
    int totalValue = 0;
    const Pieces pieces = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();
    for (const ChessTile *piece : pieces) {
        totalValue += pieceValue.at(piece->piece->getType());
    }
    return totalValue;
}

int ChessAnalyzer::getPieceValueDiff() { return getPieceValue(true) - getPieceValue(false); }

double ChessAnalyzer::evalCurrPosition(bool white) {}

double ChessAnalyzer::evalPawnStruct(bool white) {
    int totalProgress = 0;
    const Pieces allPawnPieces = white ? origBoard.getWhitePieceType(Pawn) : origBoard.getBlackPieceType(Pawn);
    for (const auto pawnPiece : allPawnPieces) {
        totalProgress += white ? pawnPiece->getY() : 7 - pawnPiece->getY();
    }
    return totalProgress;
}

void ChessAnalyzer::addToAttackedMatrix(boardMatrix &attackedBy, const bool white) {
    const Pieces colorPieces = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();

    for (const auto attackingTile : colorPieces) {
        Pieces attackedTiles;
        if (attackingTile->piece->getType() == Pawn) { // pawns extra because different attacking logic then the rest
            attackedTiles = getPawnAttackingTiles(attackingTile);
            ChessMoveLogic::filterPossibleMovesForChecks(origBoard, attackingTile, attackedTiles);
        } else {
            attackedTiles = ChessMoveLogic::getPossibleMoves(origBoard, attackingTile);
            ChessMoveLogic::filterPossibleMovesForChecks(origBoard, attackingTile, attackedTiles);
        }
        for (const auto attackedTile : attackedTiles) {
            if (white) {
                attackedBy[attackedTile->getY() * boardWidth + attackedTile->getX()].first.push_back(attackingTile);
            } else {
                attackedBy[attackedTile->getY() * boardWidth + attackedTile->getX()].second.push_back(attackingTile);
            }
        }
    }
}

void ChessAnalyzer::addToDefendMatrix(boardMatrix &defendedBy, const bool white) {
    const Pieces colorPieces = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();

    for (const auto defendingTile : colorPieces) {
        const Pieces defendedTiles = getDefendedPieces(defendingTile);
        for (const auto defendedTile : defendedTiles) {
            if (white) {
                defendedBy[defendedTile->getY() * boardWidth + defendedTile->getX()].first.push_back(defendingTile);
            } else {
                defendedBy[defendedTile->getY() * boardWidth + defendedTile->getX()].second.push_back(defendingTile);
            }
        }
    }
}

Pieces ChessAnalyzer::getPawnAttackingTiles(const ChessTile *pawnTile) {
    Pieces attackingTiles;
    const int moveY = pawnTile->piece->isWhite() ? 1 : -1;
    const int x = pawnTile->getX();
    const int y = pawnTile->getY();

    ChessTile *toTileLeft = origBoard.getTileAt(x - 1, y + moveY);
    ChessTile *toTileRight = origBoard.getTileAt(x + 1, y + moveY);

    if (toTileLeft != nullptr) {
        if (toTileLeft->piece == nullptr || pawnTile->piece->isWhite() != toTileLeft->piece->isWhite()) {
            attackingTiles.push_back(toTileLeft);
        }
    }
    if (toTileRight != nullptr) {
        if (toTileRight->piece == nullptr || pawnTile->piece->isWhite() != toTileRight->piece->isWhite()) {
            attackingTiles.push_back(toTileRight);
        }
    }
    return attackingTiles;
}

Pieces ChessAnalyzer::getDefendedPieces(const ChessTile *fromTile) {
    if (fromTile->piece == nullptr) return {};
    switch (fromTile->piece->getType()) {
        case Pawn:
            return getDefendedPiecesPawn(fromTile);
        case Rook:
            return getDefendedPiecesByDirection(fromTile, directionsRook);
        case Knight:
            return getDefendedPiecesByDirectionSingle(fromTile, directionsKnight);
        case Bishop:
            return getDefendedPiecesByDirection(fromTile, directionsBishop);
        case Queen:
            return getDefendedPiecesByDirection(fromTile, directionsQueen);
        case King:
            return getDefendedPiecesByDirectionSingle(fromTile, directionsQueen);
        default:
            return {};
    }
}

Pieces ChessAnalyzer::getDefendedPiecesPawn(const ChessTile *fromTile) {
    Pieces defendedPieces;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const int white = fromTile->piece->isWhite() ? 1 : -1;

    // capture other pawn
    addIfDefending(fromTile, origBoard.getTileAt(x + 1, y + white), defendedPieces);
    addIfDefending(fromTile, origBoard.getTileAt(x - 1, y + white), defendedPieces);
    return defendedPieces;
}

Pieces ChessAnalyzer::getDefendedPiecesByDirection(const ChessTile *fromTile, const std::vector<int8Pair> &directions) {
    Pieces defendedPieces;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (const auto &[xDirection, yDirection] : directions) {
        for (int i = 1; i < 8; i++) {
            ChessTile *nextTile = origBoard.getTileAt(x + i * xDirection, y + i * yDirection);
            if (!addIfDefending(fromTile, nextTile, defendedPieces)) break;
        }
    }
    return defendedPieces;
}

Pieces ChessAnalyzer::getDefendedPiecesByDirectionSingle(const ChessTile *fromTile, const std::vector<int8Pair> &directions) {
    Pieces defendedPieces;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (const auto &[xDirection, yDirection] : directions) {
        ChessTile *nextTile = origBoard.getTileAt(x + xDirection, y + yDirection);
        if (!addIfDefending(fromTile, nextTile, defendedPieces)) break;
    }
    return defendedPieces;
}

/// return value if continue searching
inline bool ChessAnalyzer::addIfDefending(const ChessTile *fromTile, ChessTile *toTile, Pieces &defendingMoves) {
    if (toTile == nullptr) return false; // move is outside the board
    if (toTile->piece == nullptr) return true;
    if (toTile->piece->isWhite() == fromTile->piece->isWhite()) { // is defending because it is the same color
        defendingMoves.push_back(toTile);
    }
    return false; // should be collision with different colored piece so also stop here
}
