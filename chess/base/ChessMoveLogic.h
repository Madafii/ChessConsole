#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <algorithm>
#include <functional>
#include <map>

class ChessMoveLogic {
    using enum ChessPieceType;
    using possibleMovesFunc = std::function<PieceTiles(const ChessTile &fromTile)>;

  public:
    explicit ChessMoveLogic(const ChessBoard &board);

    const ChessBoard &getChessBoard() const { return board; }

    // finds possible moves
    PieceTiles getPossibleMoves(const ChessTile &fromTile);
    PieceTiles getPossibleMovesNoCastling(const ChessTile &fromTile);
    PieceTiles getPossibleMovesUncached(const ChessTile &fromTile);
    PieceTiles getAllPossibleMoves(bool white);
    PieceTiles getAllPossibleMovesPiece(bool white, ChessPieceType piece);

    // manipulate a possible moves list
    void filterPossibleMovesForChecks(const ChessTile &fromTile, PieceTiles &possibleMoves);
    static bool addIfPossibleMove(const ChessTile &fromTile, const ChessTile &toTile, PieceTiles &possibleMoves);

    // checks for things on the board
    bool isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile);
    bool isTileAttacked(bool white, const PieceTiles &tilesToCheck);
    bool isKingChecked(bool white);
    bool isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile);
    bool isKingCheckmate(bool white);
    bool isDraw(bool white);
    bool isThreefoldRepetition() const;
    bool isDeadPosition() const;

    void resetCache() { possibleMovesCache.clear(); }

    static bool isTileFree(const PieceTiles &tilesToCheck) {
        return !std::ranges::any_of(tilesToCheck, [](const ChessTile *tile) { return !tile->hasPiece(NONE); });
    }

  private:
    const ChessBoard &board;

    std::map<const ChessTile *, PieceTiles> possibleMovesCache;

    // helpers for find possible moves
    PieceTiles getPossibleMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func);
    PieceTiles getPossibleMovesPawn(const ChessTile &fromTile);
    PieceTiles getPossibleMovesBishop(const ChessTile &fromTile);
    PieceTiles getPossibleMovesKnight(const ChessTile &fromTile);
    PieceTiles getPossibleMovesRook(const ChessTile &fromTile);
    PieceTiles getPossibleMovesQueen(const ChessTile &fromTile);
    PieceTiles getPossibleMovesKing(const ChessTile &fromTile);
    PieceTiles getPossibleMovesKingSingle(const ChessTile &fromTile);
    PieceTiles getPossibleMovesCastling(const ChessTile &fromTile);
    PieceTiles getPossibleMovesByDirection(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;
    PieceTiles getPossibleMovesByDirectionSingle(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;

    PieceTiles getLeftCastleTiles(int x, int y) { return {&board.getTileAt(x - 1, y), &board.getTileAt(x - 2, y)}; }
    PieceTiles getRightCastleTiles(int x, int y) { return {&board.getTileAt(x + 1, y), &board.getTileAt(x + 2, y)}; }
};

#endif // CHESSMOVELOGIC_H
