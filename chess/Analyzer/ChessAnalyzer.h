#ifndef CHESSANALYZER_H
#define CHESSANALYZER_H

#include "ChessBoard.h"
#include "ChessPiece.h"
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
    // a matrix with a list for each tile with attacked by pointers to pieces
    using attackMatrix = std::vector<std::pair<std::vector<ChessTile *>, std::vector<ChessTile *>>>;
    explicit ChessAnalyzer(ChessBoard &aboard);

    oStrVec getForcedCheckmate(int depth);
    oStrVec getFreePieces(const attackMatrix &matr, bool white);

    attackMatrix getAttackedMatrix();

    double evalCurrPosition(bool white);
    double evalPawnStruct(bool white);

  private:
    ChessBoard &origBoard;
    const std::map<ChessPieceType, int> pieceValue = {{King, 100000}, {Queen, 9},
                                                      {Rook, 5},      {Bishop, 3},
                                                      {Knight, 3},    {Pawn, 1}};

    void addToAttackedMatrix(attackMatrix &attackedBy, bool white);
};

#endif
