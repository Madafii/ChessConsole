#ifndef CHESSANALYZER_H
#define CHESSANALYZER_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <cstdint>
#include <map>
#include <vector>

// TODO:
// eval by:
// tiles that are attacked unique and ammount of concurrent attackers
// protected pieces and unprotected ones
// opennes of king
// total piece value
// pawn structure and how advanced they are
// move possibility for every piece

class ChessAnalyzer {
  public:
    using oStrVec = std::optional<std::vector<std::string_view>>;
    using boardMatrix = std::vector<std::pair<std::vector<ChessTile *>, std::vector<ChessTile *>>>;
    using int8Pair = std::pair<int8_t, int8_t>;

    explicit ChessAnalyzer(ChessBoard &aboard);

    oStrVec getForcedCheckmate(int depth);
    oStrVec getFreePieces(const boardMatrix &matr, bool white);

    boardMatrix getAttackedMatrix();
    boardMatrix getDefendedMatrix();

    double evalCurrPosition(bool white);
    double evalPawnStruct(bool white);

  private:
    ChessBoard &origBoard;
    const std::map<ChessPieceType, int> pieceValue = {{ChessPieceType::King, 100000}, {ChessPieceType::Queen, 9},
                                                      {ChessPieceType::Rook, 5},      {ChessPieceType::Bishop, 3},
                                                      {ChessPieceType::Knight, 3},    {ChessPieceType::Pawn, 1}};
    const std::vector<int8Pair> directionsBishop = {std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    const std::vector<int8Pair> directionsKnight = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                                    std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    const std::vector<int8Pair> directionsRook = {std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    const std::vector<int8Pair> directionsQueen = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                                   std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};

    void addToAttackedMatrix(boardMatrix &attackedBy, bool white);
    void addToDefendMatrix(boardMatrix &defendedBy, bool white);

    // helper functions for getting defender matrix.
    Pieces getDefendedPieces(const ChessTile *fromTile);
    Pieces getDefendedPiecesPawn(const ChessTile *fromTile);
    Pieces getDefendedPiecesByDirection(const ChessTile *fromTile, const std::vector<int8Pair> &directions);
    Pieces getDefendedPiecesByDirectionSingle(const ChessTile *fromTile, const std::vector<int8Pair> &directions);
    static bool addIfDefending(const ChessTile *fromTile, ChessTile *toTile, Pieces &defendingMoves);
};

#endif
