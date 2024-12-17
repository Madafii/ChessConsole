#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"

class ChessMoveLogic {
public:
    ChessMoveLogic();

    static Pieces getPossibleMoves(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesByDirection(const ChessBoard &board, const ChessTile *fromTile,
                                              const std::vector<std::pair<int, int>> &directions);
    static Pieces getPossibleMovesByDirectionSingle(const ChessBoard &board, const ChessTile *fromTile,
                                                    const std::vector<std::pair<int, int>> &directions);
    static Pieces getPossibleMovesCastling(const ChessBoard &board, const ChessTile *fromTile);

    static bool isTileAttacked(bool white, const Pieces &tilesToCheck);
    static bool isTileFree(const ChessBoard &board, const Pieces &tilesToCheck);

private:
    static Pieces getPossibleMovesPawn(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesBishop(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesKnight(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesRook(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesQueen(const ChessBoard &board, const ChessTile *fromTile);
    static Pieces getPossibleMovesKing(ChessBoard &board, const ChessTile *fromTile, bool castling = true);
    static bool addIfPossibleMove(const ChessTile *fromTile, ChessTile *toTile, Pieces &possibleMoves);
};

#endif // CHESSMOVELOGIC_H
