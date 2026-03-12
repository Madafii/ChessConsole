#include "ChessAnalyzer.h"
#include "ChessBoard.h"
#include "ChessMoveLogic.h"
#include "ChessPiece.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <string>
#include <sys/types.h>
#include <utility>

using oStrVec = ChessAnalyzer::oStrVec;
using boardMatrix = ChessAnalyzer::boardMatrix;
using evalVec = ChessAnalyzer::evalVec;
using evalLL = ChessAnalyzer::evalLL;
using enum ChessPieceType;

ChessAnalyzer::ChessAnalyzer(ChessBoard &board) : _board(board), _logic({_board}) {}

ChessAnalyzer::evalTree ChessAnalyzer::getEvalTree(uint8_t depth) {
    const auto moves = _logic.getAllLegalMoves();

    evalTree evaluationMatrix;
    evaluationMatrix.reserve(moves.size());

    for (const auto &[fromTile, toTile] : moves) {
        ChessTile &from = _board.getTileAt(fromTile->getPos());
        ChessTile &to = _board.getTileAt(toTile->getPos());
        UndoMove uMove = _board.makeMove(from, to);
        evalLL simEvalLL = getEvalNode(from, to);
        buildEvalTree(depth, simEvalLL);
        _board.undoMove(uMove);
        evaluationMatrix.push_back(simEvalLL);
    }
    return evaluationMatrix;
}

evalLL ChessAnalyzer::getEvalNode(ChessTile &fromTile, ChessTile &toTile) {
    const std::string move = fromTile.getPos() + ":" + toTile.getPos();

    // get evaluation
    const double evalValue = evalBoard();
    const double evalBonusValue = evalPiece(fromTile, toTile) * std::abs(evalValue);

    return {move, evalValue + evalBonusValue};
}

// NOLINTNEXTLINE(misc-no-recursion)
void ChessAnalyzer::buildEvalTree(uint8_t depth, evalLL &node) {
    if (depth == 0) return;

    const auto moves = _logic.getAllLegalMoves();

    node.s_nexts.reserve(moves.size());
    for (const auto &[fromTile, toTile] : moves) {
        ChessTile &from = _board.getTileAt(fromTile->getPos());
        ChessTile &to = _board.getTileAt(toTile->getPos());
        UndoMove uMove = _board.makeMove(from, to);
        evalLL simEvalLL = getEvalNode(from, to);
        node.s_nexts.emplace_back(simEvalLL);
        buildEvalTree(depth - 1, node.s_nexts.back());
        _board.undoMove(uMove);
    }
}

evalLL *ChessAnalyzer::getBestEvaluatedMove(evalTree &evalTree) {
    evalLL *bestNode = nullptr;
    bool maximizingPlayer = _board.isWhitesTurn();
    double bestValue = maximizingPlayer ? std::numeric_limits<double>::lowest() : std::numeric_limits<double>::max();

    for (auto &node : evalTree) {
        double value = minmax(node, maximizingPlayer);
        if (maximizingPlayer) {
            if (value > bestValue) {
                bestValue = value;
                bestNode = &node;
            }
        } else {
            if (value < bestValue) {
                bestValue = value;
                bestNode = &node;
            }
        }
    }
    return bestNode;
}

// NOLINTNEXTLINE(misc-no-recursion)
double ChessAnalyzer::minmax(const evalLL &node, bool maximizingPlayer) {
    if (node.s_nexts.empty()) return node.s_val;

    if (maximizingPlayer) {
        double best = std::numeric_limits<double>::lowest();
        for (const auto &child : node.s_nexts) {
            best = std::max(best, minmax(child, false));
        }
        return best;
    } else {
        double best = std::numeric_limits<double>::max();
        for (const auto &child : node.s_nexts) {
            best = std::min(best, minmax(child, true));
        }
        return best;
    }
}

/// a list of pieces being attacked but not defended
/// white: the attackers color
PieceTiles ChessAnalyzer::getFreePieces(const boardMatrix &attackMatr, const boardMatrix &defendMatr, const bool white) {
    PieceTiles freePieces;
    const PieceTiles attackedPieces = getCoveredPieces(attackMatr, white);
    const PieceTiles defendedPieces = getCoveredPieces(defendMatr, !white);
    // TODO: there is probably better algorithm for finding unique eles
    for (const ChessTile *piece : attackedPieces) {
        auto found = std::find(defendedPieces.begin(), defendedPieces.end(), piece);
        if (found != defendedPieces.end()) {
            freePieces.push_back(piece);
        }
    }
    return freePieces;
}

/// return a matrix with each tile and the corresponding attackers of that tile, where the first pair is the white attackers and the
/// second the black attackers
///
boardMatrix ChessAnalyzer::getAttackedMatrix() {
    boardMatrix attackedBy;
    addToAttackedMatrix(attackedBy, true);
    addToAttackedMatrix(attackedBy, false);
    return attackedBy;
}

boardMatrix ChessAnalyzer::getDefendedMatrix() {
    boardMatrix defendedBy;
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
PieceTiles ChessAnalyzer::getCoveredPieces(const boardMatrix &boardMat, const bool white) {
    PieceTiles converedPieces;
    for (size_t i = 0; i < boardMat.size(); i++) {
        const PieceTiles selTiles = white ? boardMat[i].first : boardMat[i].second;
        if (selTiles.empty()) continue; // not attacked
        const ChessTile &coveredTile = _board.getTileAt((int)i);
        if (coveredTile.hasPiece(NONE)) continue; // attacks empty tile
        converedPieces.push_back(&coveredTile);
    }
    return converedPieces;
}

PieceTiles ChessAnalyzer::getCoveredTiles(const boardMatrix &boardMat, const bool white) {
    PieceTiles coveredTiles;
    for (size_t i = 0; i < boardMat.size(); i++) {
        const PieceTiles selTiles = white ? boardMat[i].first : boardMat[i].second;
        if (selTiles.empty()) continue; // not attacked
        const ChessTile &coveredTile = _board.getTileAt((int)i);
        coveredTiles.push_back(&coveredTile);
    }
    return coveredTiles;
}

int ChessAnalyzer::getPieceValue(const bool white) {
    int totalValue = 0;
    const PieceTiles pieces = white ? _board.getAllWhiteTiles() : _board.getAllBlackTiles();
    for (const ChessTile *piece : pieces) {
        totalValue += _pieceValue.at(piece->getPieceType());
    }
    return totalValue;
}

int ChessAnalyzer::getPieceValueDiff(const bool white) {
    const int diff = getPieceValue(true) - getPieceValue(false);
    return white ? diff : -diff;
}

// positive means white has better board. Negative means black has better board
double ChessAnalyzer::evalBoard() {
    const double pieceValueWhite = evalPieceValue(true);
    const double pieceValueBlack = evalPieceValue(false);
    const double boardValueWhite = evalBoardValue(true);
    const double boardValueBlack = evalBoardValue(false);

    // get percantage of the difference between the players
    const double pieceValueDiff = getDiffPercentage(pieceValueWhite, pieceValueBlack);
    const double boardValueDiff = getDiffPercentage(boardValueWhite, boardValueBlack);

    // get each value considering their weight
    const double evaluatedPieceValue = weightPieceValue * pieceValueDiff;
    const double evaluatedBoardValue = weightBoardValue * boardValueDiff;

    // add the results together
    double evaluatedResult = 0;
    evaluatedResult += evaluatedPieceValue;
    evaluatedResult += evaluatedBoardValue;
    return evaluatedResult;
}

// double ChessAnalyzer::evalCurrPosition(const bool white) {
//     // TODO: should probaply all be double later but for simplicity these are int first
//
//     // board Matrixes
//     auto attackMatr = getAttackedMatrix();
//     auto defendMatr = getDefendedMatrix();
//
//     // first evaluate your position stats
//     const int pawnEval = (int)evalPawnStruct(white);
//     const int kingEval = (int)evalKingProtection(white);
//
//     auto coveredPiecesAttack = getCoveredPieces(attackMatr, white);
//     auto coveredTilesAttack = getCoveredTiles(attackMatr, white);
//     auto coveredPiecesDefend = getCoveredPieces(defendMatr, white);
//     auto coveredTilesDefend = getCoveredTiles(defendMatr, white);
//
//     const int uniquePieceAttacks = (int)coveredPiecesAttack.size();
//     const int uniqueTileAttacks = (int)coveredTilesAttack.size();
//     const int uniquePieceDefends = (int)coveredPiecesDefend.size();
//     const int uniqueTileDefends = (int)coveredTilesDefend.size();
//
//     // get opposition position stats
//     const int pawnEvalO = (int)evalPawnStruct(!white);
//     const int kingEvalO = (int)evalKingProtection(!white);
//
//     auto coveredPiecesAttackO = getCoveredPieces(attackMatr, !white);
//     auto coveredTilesAttackO = getCoveredTiles(attackMatr, !white);
//     auto coveredPiecesDefendO = getCoveredPieces(defendMatr, !white);
//     auto coveredTilesDefendO = getCoveredTiles(defendMatr, !white);
//
//     const int uniquePieceAttacksO = (int)coveredPiecesAttackO.size();
//     const int uniqueTileAttacksO = (int)coveredTilesAttackO.size();
//     const int uniquePieceDefendsO = (int)coveredPiecesDefendO.size();
//     const int uniqueTileDefendsO = (int)coveredTilesDefendO.size();
//
//     // get percantage of the difference between the players
//     const double pieceValuePerc = getDiffPercentage(getPieceValue(white), getPieceValue(!white));
//     const double pawnEvalPerc = getDiffPercentage(pawnEval, pawnEvalO);
//     const double kingEvalPerc = getDiffPercentage(kingEval, kingEvalO);
//     const double uniquePieceAttacksPerc = getDiffPercentage(uniquePieceAttacks, uniquePieceAttacksO);
//     const double uniqueTileAttacksPerc = getDiffPercentage(uniqueTileAttacks, uniqueTileAttacksO);
//     const double uniquePieceDefendsPerc = getDiffPercentage(uniquePieceDefends, uniquePieceDefendsO);
//     const double uniqueTileDefendsPerc = getDiffPercentage(uniqueTileDefends, uniqueTileDefendsO);
//
//     // get each value considering their weight
//     const double evaluatedPieceValue = weightPieceValue * pieceValuePerc;
//     const double evaluatedPawn = weightPawnValue * pawnEvalPerc;
//     const double evaluatedKing = weightKingValue * kingEvalPerc;
//     const double evaluatedUniquePieceAttacks = weightUniquePieceAttacks * uniquePieceAttacksPerc;
//     const double evaluatedUniqueTileAttacks = weightUniqueTileAttacks * uniqueTileAttacksPerc;
//     const double evaluatedUniquePieceDefends = weightUniquePieceDefends * uniquePieceDefendsPerc;
//     const double evaluatedUniqueTileDefends = weightUniqueTileDefends * uniqueTileDefendsPerc;
//     const double evaluatedKingFirstMove = weightKingFirstMove * evalKingFirstMove(white);
//
//     // add the results together
//     double evaluatedResult = 0;
//     evaluatedResult += evaluatedPieceValue;
//     evaluatedResult += evaluatedPawn;
//     evaluatedResult += evaluatedKing;
//     evaluatedResult += evaluatedKingFirstMove;
//     evaluatedResult += evaluatedUniquePieceAttacks;
//     evaluatedResult += evaluatedUniqueTileAttacks;
//     evaluatedResult += evaluatedUniquePieceAttacks;
//     evaluatedResult += evaluatedUniquePieceDefends;
//     evaluatedResult += evaluatedUniqueTileDefends;
//     return evaluatedResult;
// }

// current: only total progress of pawns on board
// TODO: eval pawns protecting each other. Penalty if pawn stands alone
double ChessAnalyzer::evalPawnStruct(const bool white) {
    int totalProgress = 0;
    const PieceTiles allPawnPieces = _board.getPieceType(white, PAWN);
    for (const auto pawnPiece : allPawnPieces) {
        totalProgress += white ? pawnPiece->getY() : 7 - pawnPiece->getY();
    }
    return totalProgress;
}

// current: only check adjacent tiles of protection by other pawns or the border
double ChessAnalyzer::evalKingProtection(const bool white) {
    int maxSideProtection = 8;
    const ChessTile *kingTile = _board.getPieceType(white, KING).front();
    const int x = kingTile->getX();
    const int y = kingTile->getY();
    for (const auto &[dirX, dirY] : ChessMoveLogic::directionsAll) {
        const ChessTile &adjTile = _board.getTileAt(x + dirX, y + dirY);
        if (adjTile.hasPiece(NONE)) maxSideProtection--;
    }
    return maxSideProtection;
}

// discourage moving the king before castling is still possible
double ChessAnalyzer::evalKingFirstMove(bool white) {
    if (_board.isCastlePossible(CastleSide::Any, white)) return -100;
    return 0;
}

double ChessAnalyzer::evalBoardValue(const bool white) {
    const PieceTiles pieces = _board.getAllTiles(white);
    double value = 0;
    for (const auto &piece : pieces) {
        int x = piece->getX();
        int y = piece->getY();
        value += boardValue[y * boardHeight + x];
    }
    return value;
}

double ChessAnalyzer::evalPieceValue(bool white) {
    const PieceTiles pieces = _board.getAllTiles(white);
    double value = 0;
    for (const auto &piece : pieces) {
        value += _pieceValue.at(piece->getPieceType());
    }
    return value;
}

double ChessAnalyzer::evalPiece(const ChessTile &pieceTile, const ChessTile &toTile) {
    switch (pieceTile.getPieceType()) {
        case KING:
            return evalKingMoves(pieceTile, toTile);
        case ROOK:
            return evalRookMoves(pieceTile);
        default:
            return 0;
    }
}

double ChessAnalyzer::evalKingMoves(const ChessTile &kingTile, const ChessTile &toTile) {
    double encourage = 0;
    // encourage castling
    if (std::abs(kingTile.getX() - toTile.getX()) >= 2) {
        encourage += 0.5;
        return encourage;
    }
    // do not encourage moving before castling was done or is still possible
    const auto &[leftRook, rightRook] = kingTile.hasWhitePiece() ? _board.getWhiteRookMoved() : _board.getBlackRookMoved();
    if (!leftRook) encourage -= 0.25;
    if (!rightRook) encourage -= 0.25;
    return encourage;
}

double ChessAnalyzer::evalRookMoves(const ChessTile &rookTile) {
    double encourage = 0;
    const auto &[leftRook, rightRook] = rookTile.hasWhitePiece() ? _board.getWhiteRookMoved() : _board.getBlackRookMoved();
    // do not encourage moving before castling was done or is still possible
    if (!leftRook) {
        if (rookTile.getX() == 0) encourage -= 0.25;
    }
    if (!rightRook) {
        if (rookTile.getX() == 7) encourage -= 0.25;
    }
    return encourage;
}

void ChessAnalyzer::addToAttackedMatrix(boardMatrix &attackedBy, const bool white) {
    const PieceTiles colorPieces = _board.getAllTiles(white);

    for (const auto &attackingTile : colorPieces) {
        PieceTiles attackedTiles;
        if (attackingTile->hasPiece(PAWN)) { // pawns extra because different attacking logic then the rest
            attackedTiles = getPawnAttackingTiles(*attackingTile);
            _logic.filterLegalMoves(*attackingTile, attackedTiles);
        } else {
            attackedTiles = _logic.getLegalMoves(*attackingTile);
        }
        for (const auto &attackedTile : attackedTiles) {
            if (white) {
                attackedBy[attackedTile->getY() * boardWidth + attackedTile->getX()].first.push_back(attackingTile);
            } else {
                attackedBy[attackedTile->getY() * boardWidth + attackedTile->getX()].second.push_back(attackingTile);
            }
        }
    }
}

void ChessAnalyzer::addToDefendMatrix(boardMatrix &defendedBy, const bool white) {
    const PieceTiles colorPieces = white ? _board.getAllWhiteTiles() : _board.getAllBlackTiles();

    for (const auto defendingTile : colorPieces) {
        const PieceTiles defendedTiles = getDefendedPieces(*defendingTile);
        for (const auto &defendedTile : defendedTiles) {
            if (white) {
                defendedBy[defendedTile->getY() * boardWidth + defendedTile->getX()].first.push_back(defendingTile);
            } else {
                defendedBy[defendedTile->getY() * boardWidth + defendedTile->getX()].second.push_back(defendingTile);
            }
        }
    }
}

PieceTiles ChessAnalyzer::getPawnAttackingTiles(const ChessTile &pawnTile) {
    PieceTiles attackingTiles;
    const int moveY = pawnTile.hasWhitePiece() ? 1 : -1;
    const int x = pawnTile.getX();
    const int y = pawnTile.getY();

    const int leftX = x - 1;
    const int rightX = x + 1;
    const int y2 = y + moveY;
    if (ChessBoard::validTilePos(leftX, y2)) {
        const ChessTile &toTileLeft = _board.getTileAt(leftX, y2);
        if (!toTileLeft.hasPiece() || pawnTile.hasWhitePiece() != toTileLeft.hasWhitePiece()) {
            attackingTiles.push_back(&toTileLeft);
        }
    }
    if (ChessBoard::validTilePos(rightX, y2)) {
        const ChessTile &toTileRight = _board.getTileAt(rightX, y2);
        if (!toTileRight.hasPiece() || pawnTile.hasWhitePiece() != toTileRight.hasWhitePiece()) {
            attackingTiles.push_back(&toTileRight);
        }
    }

    return attackingTiles;
}

PieceTiles ChessAnalyzer::getDefendedPieces(const ChessTile &fromTile) {
    if (!fromTile.hasPiece()) return {};
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getDefendedPiecesPawn(fromTile);
        case ROOK:
            return getDefendedPiecesByDirection(fromTile, ChessMoveLogic::directionsStraight);
        case KNIGHT:
            return getDefendedPiecesByDirectionSingle(fromTile, ChessMoveLogic::directionsKnight);
        case BISHOP:
            return getDefendedPiecesByDirection(fromTile, ChessMoveLogic::directionsDiagonal);
        case QUEEN:
            return getDefendedPiecesByDirection(fromTile, ChessMoveLogic::directionsAll);
        case KING:
            return getDefendedPiecesByDirectionSingle(fromTile, ChessMoveLogic::directionsAll);
        default:
            return {};
    }
}

PieceTiles ChessAnalyzer::getDefendedPiecesPawn(const ChessTile &fromTile) {
    PieceTiles defendedPieces;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    const int moveY = fromTile.hasWhitePiece() ? 1 : -1;

    // capture other pawn
    const int leftX = x - 1;
    const int rightX = x + 1;
    const int y2 = y + moveY;
    if (ChessBoard::validTilePos(leftX, y2)) {
        addIfDefending(fromTile, _board.getTileAt(leftX, y2), defendedPieces);
    }
    if (ChessBoard::validTilePos(rightX, y2)) {
        addIfDefending(fromTile, _board.getTileAt(rightX, y2), defendedPieces);
    }
    return defendedPieces;
}

/// return value if continue searching
bool ChessAnalyzer::addIfDefending(const ChessTile &fromTile, const ChessTile &toTile, PieceTiles &defendingMoves) {
    if (!toTile.hasPiece()) return true;                      // empty tile
    if (toTile.hasWhitePiece() == fromTile.hasWhitePiece()) { // is defending because it is the same color
        defendingMoves.push_back(&toTile);
    }
    return false; // should be collision with different colored piece so also stop here
}

double ChessAnalyzer::getDiffPercentage(const double player, const double opponent) {
    if (player == 0 && opponent == 0) return 0;
    return 100 / std::max(player, opponent) * (player - opponent);
}
