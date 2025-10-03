#include "ChessAnalyzer.h"
#include "ChessBoard.h"
#include "ChessInterface.h"
#include "ChessMoveLogic.h"
#include "ChessPiece.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

using oStrVec = ChessAnalyzer::oStrVec;
using boardMatrix = ChessAnalyzer::boardMatrix;
using enum ChessPieceType;

ChessAnalyzer::ChessAnalyzer(const ChessBoard &board)
    : origBoard(board), chessLogic(ChessMoveLogic(origBoard)) {}

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
        std::cout << std::format("can take: {} at x:{} y:{}", piece->getPiece().getLongName(), piece->getX() + 1, piece->getY() + 1)
                  << std::endl;
    }
}

/// returns a vector of moves if a forced checkmate is possible
oStrVec ChessAnalyzer::getForcedCheckmate(int depth) {
    const bool white = origBoard.isWhitesTurn();
    const Pieces allMoves = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();

    return std::nullopt;
}

/// TODO: evaluates the board position for each possible move. Also adding some depth, which returns new evaluations. Should prioritize
/// worst outcome after opponent moves again for evaluating the move before that
// std::vector<std::pair<double, std::string>> ChessAnalyzer::getBestEvalMoves() {
//     const bool white = origBoard.isWhitesTurn();
//
//     const Pieces colorPieces = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();
//     std::vector<std::pair<double, std::string>> evalMovesList;
//     for (const ChessTile *fromMove : colorPieces) {
//         const std::string fromMoveName = fromMove->getMove();
//         Pieces possibleMoves = chessLogic.getPossibleMoves(*fromMove);
//         chessLogic.filterPossibleMovesForChecks(*fromMove, possibleMoves);
//         for (const ChessTile *move : possibleMoves) {
//             // make a copy of the board play the move and evaluate the position
//             ChessBoard simulateBoard(origBoard);
//             std::string moveName = fromMoveName + ":" + move->getMove();
//             simulateBoard.handleMoveInput(moveName);
//             ChessAnalyzer boardAnalyzer(simulateBoard);
//             const double evalValue = boardAnalyzer.evalCurrPosition(white);
//             evalMovesList.emplace_back(evalValue, moveName);
//         }
//     }
//
//     // sort which position has the best evaluation
//     std::sort(evalMovesList.begin(), evalMovesList.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
//     return evalMovesList;
// }

/// only works for depth of one for now
// std::vector<std::pair<double, std::string>> ChessAnalyzer::getBestEvalMoves(const int depth) {
//     bool white = origBoard.isWhitesTurn();
//     auto origBestMoves = getBestEvalMoves();
//     /*auto currBestMoves = origBestMoves;*/
//     for (int i = 0; i <= depth; i++) {
//         // change player
//         white = !white;
//         std::vector<std::pair<double, std::string>> evalMovesList;
//         std::pair<double, std::string> worstMove(10000, ""); // dummy value that will be overwritten
//         for (auto &[evalValue, moveName] : origBestMoves) {
//             // make move from previous evaluation
//             ChessBoard simulateBoard(origBoard);
//             simulateBoard.handleMoveInput(moveName);
//             // get the new best moves
//             ChessAnalyzer boardAnalyzer(simulateBoard);
//             auto simBestMoves = boardAnalyzer.getBestEvalMoves();
//             // get the worst move
//             const auto simWorstMove = simBestMoves.back();
//             evalValue = std::min(evalValue, simWorstMove.first);
//         }
//     }
//
//     std::sort(origBestMoves.begin(), origBestMoves.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
//     return origBestMoves;
// }

/// a list of pieces being attacked but not defended
/// white: the attackers color
Pieces ChessAnalyzer::getFreePieces(const boardMatrix &attackMatr, const boardMatrix &defendMatr, const bool white) {
    Pieces freePieces;
    const Pieces attackedPieces = getCoveredPieces(attackMatr, white);
    const Pieces defendedPieces = getCoveredPieces(defendMatr, !white);
    // TODO: there is probably better algorithm for finding unique eles
    for (const ChessTile *piece : attackedPieces) {
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
Pieces ChessAnalyzer::getCoveredPieces(const boardMatrix &boardMat, const bool white) {
    Pieces converedPieces;
    for (size_t i = 0; i < boardMat.size(); i++) {
        const Pieces selTiles = white ? boardMat[i].first : boardMat[i].second;
        if (selTiles.empty()) continue; // not attacked
        const ChessTile &coveredTile = origBoard.getTileAt((int)i);
        if (coveredTile.hasPiece(NONE)) continue; // attacks empty tile
        converedPieces.push_back(&coveredTile);
    }
    return converedPieces;
}

Pieces ChessAnalyzer::getCoveredTiles(const boardMatrix &boardMat, const bool white) {
    Pieces coveredTiles;
    for (size_t i = 0; i < boardMat.size(); i++) {
        const Pieces selTiles = white ? boardMat[i].first : boardMat[i].second;
        if (selTiles.empty()) continue; // not attacked
        const ChessTile &coveredTile = origBoard.getTileAt((int)i);
        coveredTiles.push_back(&coveredTile);
    }
    return coveredTiles;
}

int ChessAnalyzer::getPieceValue(const bool white) {
    int totalValue = 0;
    const Pieces pieces = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();
    for (const ChessTile *piece : pieces) {
        totalValue += pieceValue.at(piece->getPieceType());
    }
    return totalValue;
}

int ChessAnalyzer::getPieceValueDiff(const bool white) {
    const int diff = getPieceValue(true) - getPieceValue(false);
    return white ? diff : -diff;
}


double ChessAnalyzer::evalCurrPosition(const bool white) {
    // TODO: should probaply all be double later but for simplicity these are int first

    // board Matrixes
    auto attackMatr = getAttackedMatrix();
    auto defendMatr = getDefendedMatrix();

    // first evaluate your position stats
    const int pawnEval = (int)evalPawnStruct(white);
    const int kingEval = (int)evalKingProtection(white);

    auto coveredPiecesAttack = getCoveredPieces(attackMatr, white);
    auto coveredTilesAttack = getCoveredTiles(attackMatr, white);
    auto coveredPiecesDefend = getCoveredPieces(defendMatr, white);
    auto coveredTilesDefend = getCoveredTiles(defendMatr, white);

    const int uniquePieceAttacks = (int)coveredPiecesAttack.size();
    const int uniqueTileAttacks = (int)coveredTilesAttack.size();
    const int uniquePieceDefends = (int)coveredPiecesDefend.size();
    const int uniqueTileDefends = (int)coveredTilesDefend.size();

    // get opposition position stats
    const int pawnEvalO = (int)evalPawnStruct(!white);
    const int kingEvalO = (int)evalKingProtection(!white);

    auto coveredPiecesAttackO = getCoveredPieces(attackMatr, !white);
    auto coveredTilesAttackO = getCoveredTiles(attackMatr, !white);
    auto coveredPiecesDefendO = getCoveredPieces(defendMatr, !white);
    auto coveredTilesDefendO = getCoveredTiles(defendMatr, !white);

    const int uniquePieceAttacksO = (int)coveredPiecesAttackO.size();
    const int uniqueTileAttacksO = (int)coveredTilesAttackO.size();
    const int uniquePieceDefendsO = (int)coveredPiecesDefendO.size();
    const int uniqueTileDefendsO = (int)coveredTilesDefendO.size();

    // get percantage of the difference between the players
    const double pieceValuePerc = getDiffPercentage(getPieceValue(white), getPieceValue(!white));
    const double pawnEvalPerc = getDiffPercentage(pawnEval, pawnEvalO);
    const double kingEvalPerc = getDiffPercentage(kingEval, kingEvalO);
    const double uniquePieceAttacksPerc = getDiffPercentage(uniquePieceAttacks, uniquePieceAttacksO);
    const double uniqueTileAttacksPerc = getDiffPercentage(uniqueTileAttacks, uniqueTileAttacksO);
    const double uniquePieceDefendsPerc = getDiffPercentage(uniquePieceDefends, uniquePieceDefendsO);
    const double uniqueTileDefendsPerc = getDiffPercentage(uniqueTileDefends, uniqueTileDefendsO);

    // get each value considering their weight
    const double evaluatedPieceValue = weightPieceValue * pieceValuePerc;
    const double evaluatedPawn = weightPawnValue * pawnEvalPerc;
    const double evaluatedKing = weightKingValue * kingEvalPerc;
    const double evaluatedUniquePieceAttacks = weightUniquePieceAttacks * uniquePieceAttacksPerc;
    const double evaluatedUniqueTileAttacks = weightUniqueTileAttacks * uniqueTileAttacksPerc;
    const double evaluatedUniquePieceDefends = weightUniquePieceDefends * uniquePieceDefendsPerc;
    const double evaluatedUniqueTileDefends = weightUniqueTileDefends * uniqueTileDefendsPerc;
    const double evaluatedKingFirstMove = weightKingFirstMove * evalKingFirstMove(white);

    // add the results together
    double evaluatedResult = 0;
    evaluatedResult += evaluatedPieceValue;
    evaluatedResult += evaluatedPawn;
    evaluatedResult += evaluatedKing;
    evaluatedResult += evaluatedKingFirstMove;
    evaluatedResult += evaluatedUniquePieceAttacks;
    evaluatedResult += evaluatedUniqueTileAttacks;
    evaluatedResult += evaluatedUniquePieceAttacks;
    evaluatedResult += evaluatedUniquePieceDefends;
    evaluatedResult += evaluatedUniqueTileDefends;
    return evaluatedResult;
}

// current: only total progress of pawns on board
// TODO: eval pawns protecting each other. Penalty if pawn stands alone
double ChessAnalyzer::evalPawnStruct(const bool white) {
    int totalProgress = 0;
    const Pieces allPawnPieces = origBoard.getPieceType(white, PAWN);
    for (const auto pawnPiece : allPawnPieces) {
        totalProgress += white ? pawnPiece->getY() : 7 - pawnPiece->getY();
    }
    return totalProgress;
}

// current: only check adjacent tiles of protection by other pawns or the border
double ChessAnalyzer::evalKingProtection(const bool white) {
    int maxSideProtection = 8;
    const ChessTile *kingTile = origBoard.getPieceType(white, KING).front();
    const int x = kingTile->getX();
    const int y = kingTile->getY();
    for (const auto &[dirX, dirY] : directionsQueen) {
        const ChessTile &adjTile = origBoard.getTileAt(x + dirX, y + dirY);
        if (adjTile.hasPiece(NONE)) maxSideProtection--;
    }
    return maxSideProtection;
}

// discourage moving the king before castling is still possible
double ChessAnalyzer::evalKingFirstMove(bool white) {
    if (origBoard.isCastlePossible(CastleSide::ANY, white)) return -100;
    return 0;
}

double ChessAnalyzer::evalBoardValue(const bool white) {
    const Pieces pieces = origBoard.getAllTiles(white);
    double value = 0;
    for (const auto &piece : pieces) {
        int x = piece->getX();
        int y = piece->getY();
        value += boardValue[y * boardHeight + x];
    }
    return value;
}

double ChessAnalyzer::evalPieceValue(bool white) {
    const Pieces pieces = origBoard.getAllTiles(white);
    double value = 0;
    for (const auto &piece : pieces) {
        value += pieceValue.at(piece->getPieceType());
    }
    return value;
}

void ChessAnalyzer::addToAttackedMatrix(boardMatrix &attackedBy, const bool white) {
    const Pieces colorPieces = origBoard.getAllTiles(white);

    for (const auto &attackingTile : colorPieces) {
        Pieces attackedTiles;
        if (attackingTile->hasPiece(PAWN)) { // pawns extra because different attacking logic then the rest
            attackedTiles = getPawnAttackingTiles(*attackingTile);
            chessLogic.filterPossibleMovesForChecks(*attackingTile, attackedTiles);
        } else {
            attackedTiles = chessLogic.getPossibleMovesUncached(*attackingTile);
            chessLogic.filterPossibleMovesForChecks(*attackingTile, attackedTiles);
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
    const Pieces colorPieces = white ? origBoard.getAllWhiteTiles() : origBoard.getAllBlackTiles();

    for (const auto defendingTile : colorPieces) {
        const Pieces defendedTiles = getDefendedPieces(*defendingTile);
        for (const auto &defendedTile : defendedTiles) {
            if (white) {
                defendedBy[defendedTile->getY() * boardWidth + defendedTile->getX()].first.push_back(defendingTile);
            } else {
                defendedBy[defendedTile->getY() * boardWidth + defendedTile->getX()].second.push_back(defendingTile);
            }
        }
    }
}

Pieces ChessAnalyzer::getPawnAttackingTiles(const ChessTile &pawnTile) {
    Pieces attackingTiles;
    const int moveY = pawnTile.hasWhitePiece() ? 1 : -1;
    const int x = pawnTile.getX();
    const int y = pawnTile.getY();

    const int leftX = x - 1;
    const int rightX = x + 1;
    const int y2 = y + moveY;
    if (ChessBoard::validTilePos(leftX, y2)) {
        const ChessTile &toTileLeft = origBoard.getTileAt(leftX, y2);
        if (toTileLeft.hasPiece(NONE) || pawnTile.hasWhitePiece() != toTileLeft.hasWhitePiece()) {
            attackingTiles.push_back(&toTileLeft);
        }
    }
    if (ChessBoard::validTilePos(rightX, y2)) {
        const ChessTile &toTileRight = origBoard.getTileAt(rightX, y2);
        if (toTileRight.hasPiece(NONE) || pawnTile.hasWhitePiece() != toTileRight.hasWhitePiece()) {
            attackingTiles.push_back(&toTileRight);
        }
    }

    return attackingTiles;
}

Pieces ChessAnalyzer::getDefendedPieces(const ChessTile &fromTile) {
    if (fromTile.hasPiece(NONE)) return {};
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getDefendedPiecesPawn(fromTile);
        case ROOK:
            return getDefendedPiecesByDirection(fromTile, directionsRook);
        case KNIGHT:
            return getDefendedPiecesByDirectionSingle(fromTile, directionsKnight);
        case BISHOP:
            return getDefendedPiecesByDirection(fromTile, directionsBishop);
        case QUEEN:
            return getDefendedPiecesByDirection(fromTile, directionsQueen);
        case KING:
            return getDefendedPiecesByDirectionSingle(fromTile, directionsQueen);
        default:
            return {};
    }
}

Pieces ChessAnalyzer::getDefendedPiecesPawn(const ChessTile &fromTile) {
    Pieces defendedPieces;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    const int moveY = fromTile.hasWhitePiece() ? 1 : -1;

    // capture other pawn
    const int leftX = x - 1;
    const int rightX = x + 1;
    const int y2 = y + moveY;
    if (ChessBoard::validTilePos(leftX, y2)) {
        addIfDefending(fromTile, origBoard.getTileAt(leftX, y2), defendedPieces);
    }
    if (ChessBoard::validTilePos(rightX, y2)) {
        addIfDefending(fromTile, origBoard.getTileAt(rightX, y2), defendedPieces);
    }
    return defendedPieces;
}

Pieces ChessAnalyzer::getDefendedPiecesByDirection(const ChessTile &fromTile, const std::vector<int8Pair> &directions) {
    Pieces defendedPieces;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    for (const auto &[xDirection, yDirection] : directions) {
        for (int i = 1; i < 8; i++) {
            const int currX = x + i * xDirection;
            const int currY = y + i * yDirection;
            if (!ChessBoard::validTilePos(currX, currY)) break; // no more tiles this direction
            const ChessTile &nextTile = origBoard.getTileAt(currX, currY);
            if (!addIfDefending(fromTile, nextTile, defendedPieces)) break; // path blocked
        }
    }
    return defendedPieces;
}

Pieces ChessAnalyzer::getDefendedPiecesByDirectionSingle(const ChessTile &fromTile, const std::vector<int8Pair> &directions) {
    Pieces defendedPieces;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    for (const auto &[xDirection, yDirection] : directions) {
        if (!ChessBoard::validTilePos(x + xDirection, y + yDirection)) continue;
        const ChessTile &nextTile = origBoard.getTileAt(x + xDirection, y + yDirection);
        addIfDefending(fromTile, nextTile, defendedPieces);
    }
    return defendedPieces;
}

/// return value if continue searching
bool ChessAnalyzer::addIfDefending(const ChessTile &fromTile, const ChessTile &toTile, Pieces &defendingMoves) {
    if (toTile.hasPiece(NONE)) return true; // empty tile
    if (toTile.hasWhitePiece() == fromTile.hasWhitePiece()) { // is defending because it is the same color
        defendingMoves.push_back(&toTile);
    }
    return false; // should be collision with different colored piece so also stop here
}

double ChessAnalyzer::getDiffPercentage(const double player, const double opponent) {
    if (player == 0 && opponent == 0) return 0;
    return 100 / std::max(player, opponent) * (player - opponent);
}
