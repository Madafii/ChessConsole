#ifndef CHESSANALYZER_H
#define CHESSANALYZER_H

#include "ChessBoard.h"
#include "ChessMoveLogic.h"
#include "ChessPiece.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <string_view>
#include <vector>

// TODO:
// eval by:
// tiles that are attacked unique and ammount of concurrent attackers | partly done
// protected pieces and unprotected ones | partly done
// opennes of king
// total piece value | done
// pawn structure and how advanced they are
// move possibility for every piece | done
// free pieces | done
// don't move king before castlinng | done
// don't repeat moves that much | todo
// option to make bot more aggressive | todo
// try move every piece once first | todo

class ChessAnalyzer {
  public:
    using oStrVec = std::optional<std::vector<std::string_view>>;
    using boardMatrix = std::array<std::pair<std::vector<const ChessTile *>, std::vector<const ChessTile *>>, boardSize>;
    using valueMatrix = std::array<double, boardSize>;
    using int8Pair = std::pair<int8_t, int8_t>;

    explicit ChessAnalyzer(const ChessBoard &board);

    std::string startTerminalAnalyzer();

    oStrVec getForcedCheckmate(int depth);
    std::vector<std::pair<double, std::string>> getEvalMoves();
    std::vector<std::pair<double, std::string>> getBestEvalMoves(int depth);
    // std::vector<std::pair<double, std::string>> getBestEvalMoves(int depth);

    PieceTiles getFreePieces(const boardMatrix &attackMatr, const boardMatrix &defendMatr, bool white);
    // get matrix covering all tiles by what piece is attacking it
    boardMatrix getAttackedMatrix();
    // get matrix covering all defended pieces by who is defending it
    boardMatrix getDefendedMatrix();

    [[nodiscard]] static size_t boardMatrixSize(const boardMatrix &boardMatr, bool white);
    [[nodiscard]] PieceTiles getCoveredPieces(const boardMatrix &boardMat, bool white);
    [[nodiscard]] PieceTiles getCoveredTiles(const boardMatrix &boardMat, bool white);

    [[nodiscard]] int getPieceValue(bool white);
    [[nodiscard]] int getPieceValueDiff(bool white);

    double evalBoard();

    // single evals
    double evalBoardValue(bool white);
    double evalPieceValue(bool white);
    double evalCastle(bool white);

    // not used now
    double evalCurrPosition(bool white);
    double evalPawnStruct(bool white);
    double evalKingProtection(bool white);
    double evalKingFirstMove(bool white);

  private:
    const ChessBoard &origBoard;
    ChessMoveLogic chessLogic;
    const std::map<ChessPieceType, int> pieceValue = {{ChessPieceType::KING, 0},   {ChessPieceType::QUEEN, 9},  {ChessPieceType::ROOK, 5},
                                                      {ChessPieceType::BISHOP, 3}, {ChessPieceType::KNIGHT, 3}, {ChessPieceType::PAWN, 1},
                                                      {ChessPieceType::NONE, 0}};
    static constexpr std::array<uint8_t, boardSize> boardValue = {0, 1, 1, 1, 1, 1, 1, 0,
                                                                  1, 2, 2, 2, 2, 2, 2, 1,
                                                                  1, 2, 3, 3, 3, 3, 2, 1,
                                                                  1, 2, 3, 3, 3, 3, 2, 1,
                                                                  1, 2, 3, 3, 3, 3, 2, 1,
                                                                  1, 2, 3, 3, 3, 3, 2, 1,
                                                                  1, 2, 2, 2, 2, 2, 2, 1,
                                                                  0, 1, 1, 1, 1, 1, 1, 0};
    const std::vector<int8Pair> directionsBishop = {std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    const std::vector<int8Pair> directionsKnight = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                                    std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    const std::vector<int8Pair> directionsRook = {std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    const std::vector<int8Pair> directionsQueen = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                                   std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};

    // ------ weights for evaluation should amount to one (100)-------
    static constexpr double weightPieceValue = 0.9;
    static constexpr double weightBoardValue = 0.1;
    // static constexpr double weightPawnValue = 0.03;
    // static constexpr double weightKingValue = 0.005;
    // static constexpr double weightKingFirstMove = 0.02;
    // static constexpr double weightUniquePieceAttacks = 0.05;
    // static constexpr double weightUniqueTileAttacks = 0.04;
    // static constexpr double weightUniquePieceDefends = 0.015;
    // static constexpr double weightUniqueTileDefends = 0.025;
    // ----------------------------------------------------------------

    void addToAttackedMatrix(boardMatrix &attackedBy, bool white);
    void addToDefendMatrix(boardMatrix &defendedBy, bool white);

    // helper for attacker matrix
    PieceTiles getPawnAttackingTiles(const ChessTile &pawnTile);

    // helper for defender matrix
    PieceTiles getDefendedPieces(const ChessTile &fromTile);
    PieceTiles getDefendedPiecesPawn(const ChessTile &fromTile);
    PieceTiles getDefendedPiecesByDirection(const ChessTile &fromTile, const std::vector<int8Pair> &directions);
    PieceTiles getDefendedPiecesByDirectionSingle(const ChessTile &fromTile, const std::vector<int8Pair> &directions);
    static bool addIfDefending(const ChessTile &fromTile, const ChessTile &toTile, PieceTiles &defendingMoves);

    // eval helper
    [[nodiscard]] static double getDiffPercentage(double player, double opponent);
};

#endif
