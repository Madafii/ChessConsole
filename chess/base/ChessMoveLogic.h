#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <functional>

using possibleMovesFunc = std::function<Pieces(const ChessBoard &board, const ChessTile &fromTile)>;

class ChessMoveLogic {
    using enum ChessPieceType;
  public:
    ChessMoveLogic() = default;

    // finds possible moves
    static Pieces getPossibleMoves(ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesUncached(ChessBoard &board, const ChessTile &fromTile);
    static Pieces getAllPossibleMoves(ChessBoard &board, bool white);

    // manipulate a possible moves list
    static void filterPossibleMovesForChecks(ChessBoard &board, const ChessTile &fromTile, Pieces &possibleMoves);
    static bool addIfPossibleMove(const ChessTile &fromTile, ChessTile &toTile, Pieces &possibleMoves);

    // checks for things on the board
    static bool isInputMovePossible(ChessBoard &board, const ChessTile &fromTile, const ChessTile &toTile);
    static bool isTileAttacked(ChessBoard &board, bool white, const Pieces &tilesToCheck);
    static bool isTileFree(const ChessBoard &board, const Pieces &tilesToCheck);
    static bool isKingChecked(ChessBoard &board, bool white, bool cached = true);
    static bool isKingCheckedAfterMove(ChessBoard &board, const ChessTile &fromTile, ChessTile &toTile);
    static bool isKingCheckmate(ChessBoard &board);
    static bool isDraw(ChessBoard &board);
    static bool isThreefoldRepetition(const ChessBoard &board);
    static bool isDeadPosition(const ChessBoard &board);

  private:
    // helpers for find possible moves
    static Pieces getPossibleMovesCached(ChessBoard &board, const ChessTile &fromTile, const possibleMovesFunc &func);
    static Pieces getPossibleMovesPawn(const ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesBishop(const ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesKnight(const ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesRook(const ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesQueen(const ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesKing(ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesKingSingle(ChessBoard &board, const ChessTile &fromTile, bool cache = true);
    static Pieces getPossibleMovesCastling(ChessBoard &board, const ChessTile &fromTile);
    static Pieces getPossibleMovesByDirection(const ChessBoard &board, const ChessTile &fromTile,
                                              const std::vector<std::pair<int, int>> &directions);
    static Pieces getPossibleMovesByDirectionSingle(const ChessBoard &board, const ChessTile &fromTile,
                                                    const std::vector<std::pair<int, int>> &directions);
};

#endif // CHESSMOVELOGIC_H
