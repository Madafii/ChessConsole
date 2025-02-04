#ifndef CHESSANALYZER_H
#define CHESSANALYZER_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <cstddef>
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

class ChessAnalyzer {
  public:
    using oStrVec = std::optional<std::vector<std::string_view>>;
    using boardMatrix = std::vector<std::pair<std::vector<ChessTile *>, std::vector<ChessTile *>>>;
    using int8Pair = std::pair<int8_t, int8_t>;

    explicit ChessAnalyzer(ChessBoard &aboard);

    std::string startTerminalAnalyzer();

    oStrVec getForcedCheckmate(int depth);
    std::vector<std::string> getBestEvalMoves();

    Pieces getFreePieces(const boardMatrix &attackMatr, const boardMatrix &defendMatr, bool white);

    boardMatrix getAttackedMatrix();
    boardMatrix getDefendedMatrix();
    [[nodiscard]] static size_t boardMatrixSize(const boardMatrix &boardMatr, bool white);
    [[nodiscard]] Pieces getCoveredPieces(const boardMatrix &boardMat, bool white);
    [[nodiscard]] Pieces getCoveredTiles(const boardMatrix &boardMat, bool white);

    [[nodiscard]] int getPieceValue(bool white);
    [[nodiscard]] int getPieceValueDiff(bool white);

    double evalCurrPosition(bool white);
    double evalPawnStruct(bool white);
    double evalKingProtection(bool white);

  private:
    ChessBoard &origBoard;
    const std::map<ChessPieceType, int> pieceValue = {{King, 0}, {Queen, 9}, {Rook, 5}, {Bishop, 3}, {Knight, 3}, {Pawn, 1}};
    const std::vector<int8Pair> directionsBishop = {std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    const std::vector<int8Pair> directionsKnight = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                                    std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    const std::vector<int8Pair> directionsRook = {std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    const std::vector<int8Pair> directionsQueen = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                                   std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};

    void addToAttackedMatrix(boardMatrix &attackedBy, bool white);
    void addToDefendMatrix(boardMatrix &defendedBy, bool white);

    // helper for attacker matrix
    Pieces getPawnAttackingTiles(const ChessTile *pawnTile);

    // helper for defender matrix
    Pieces getDefendedPieces(const ChessTile *fromTile);
    Pieces getDefendedPiecesPawn(const ChessTile *fromTile);
    Pieces getDefendedPiecesByDirection(const ChessTile *fromTile, const std::vector<int8Pair> &directions);
    Pieces getDefendedPiecesByDirectionSingle(const ChessTile *fromTile, const std::vector<int8Pair> &directions);
    static bool addIfDefending(const ChessTile *fromTile, ChessTile *toTile, Pieces &defendingMoves);

    // eval helper
    double getDiffPercentage(double player, double opponent);
};

#endif
