#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"

class ChessMoveLogic {
  public:
    ChessMoveLogic();

    // finds possible moves
    static Pieces getPossibleMoves(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesPawn(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesBishop(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesKnight(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesRook(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesQueen(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesKing(const ChessBoard &board, const ChessTile *fromTile, bool castling = true);
    static Pieces getPossibleMovesCastling(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesByDirection(const ChessBoard &board, const ChessTile *fromTile,
                                              const std::vector<std::pair<int, int>> &directions);
    static Pieces getPossibleMovesByDirectionSingle(const ChessBoard &board, const ChessTile *fromTile,
                                                    const std::vector<std::pair<int, int>> &directions);
    static Pieces getAllPossibleMoves(const ChessBoard &board, const bool white);

    // manipulate a possible moves list
    static void filterPossibleMovesForChecks(const ChessBoard &board, const ChessTile *fromTile, Pieces &possibleMoves);
    static bool addIfPossibleMove(const ChessTile *fromTile, ChessTile *toTile, Pieces &possibleMoves);

    // checks for things on the board
    static bool isInputMovePossible(const ChessBoard &board, const ChessTile *fromTile, const ChessTile *toTile);
    static bool isTileAttacked(const ChessBoard &board, bool white, const Pieces &tilesToCheck);
    static bool isTileFree(const ChessBoard &board, const Pieces &tilesToCheck);
    static bool isKingChecked(const ChessBoard &board, const bool white);
    static bool isKingChecked(const ChessBoard &board, const ChessTile *fromTile, ChessTile *toTile);
    static bool isKingCheckmate(const ChessBoard &board);
    static bool isDraw(const ChessBoard &board);
    static bool isThreefoldRepetition(const ChessBoard &board);
    static bool isDeadPosition(const ChessBoard &board);
};

#endif // CHESSMOVELOGIC_H
