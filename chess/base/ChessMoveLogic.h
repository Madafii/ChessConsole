#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <algorithm>
#include <functional>
#include <map>

class ChessMoveLogic {
    using enum ChessPieceType;
    using possibleMovesFunc = std::function<Pieces(const ChessTile &fromTile)>;

  public:
    explicit ChessMoveLogic(const ChessBoard &board);

    const ChessBoard &getChessBoard() const { return board; }

    // finds possible moves
    Pieces getPossibleMoves(const ChessTile &fromTile);
    Pieces getPossibleMovesNoCastling(const ChessTile &fromTile);
    Pieces getPossibleMovesUncached(const ChessTile &fromTile);
    Pieces getAllPossibleMoves(bool white);
    Pieces getAllPossibleMovesPiece(bool white, ChessPieceType piece);

    // manipulate a possible moves list
    void filterPossibleMovesForChecks(const ChessTile &fromTile, Pieces &possibleMoves);
    static bool addIfPossibleMove(const ChessTile &fromTile, const ChessTile &toTile, Pieces &possibleMoves);

    // checks for things on the board
    bool isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile);
    bool isTileAttacked(bool white, const Pieces &tilesToCheck);
    bool isKingChecked(bool white);
    bool isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile);
    bool isKingCheckmate(bool white);
    bool isDraw(bool white);
    bool isThreefoldRepetition() const;
    bool isDeadPosition() const;

    void resetCache() { possibleMovesCache.clear(); }

    static bool isTileFree(const Pieces &tilesToCheck) {
        return !std::ranges::any_of(tilesToCheck, [](const ChessTile *tile) { return !tile->hasPiece(NONE); });
    }

  private:
    const ChessBoard &board;

    std::map<const ChessTile *, Pieces> possibleMovesCache;

    // helpers for find possible moves
    Pieces getPossibleMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func);
    Pieces getPossibleMovesPawn(const ChessTile &fromTile);
    Pieces getPossibleMovesBishop(const ChessTile &fromTile);
    Pieces getPossibleMovesKnight(const ChessTile &fromTile);
    Pieces getPossibleMovesRook(const ChessTile &fromTile);
    Pieces getPossibleMovesQueen(const ChessTile &fromTile);
    Pieces getPossibleMovesKing(const ChessTile &fromTile);
    Pieces getPossibleMovesKingSingle(const ChessTile &fromTile);
    Pieces getPossibleMovesCastling(const ChessTile &fromTile);
    Pieces getPossibleMovesByDirection(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;
    Pieces getPossibleMovesByDirectionSingle(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;

    Pieces getLeftCastleTiles(int x, int y) { return {&board.getTileAt(x - 1, y), &board.getTileAt(x - 2, y)}; }
    Pieces getRightCastleTiles(int x, int y) { return {&board.getTileAt(x + 1, y), &board.getTileAt(x + 2, y)}; }
};

#endif // CHESSMOVELOGIC_H
