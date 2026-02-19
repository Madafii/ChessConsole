#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <algorithm>
#include <functional>
#include <map>

// TODO: rework caching
// rework whole king thing. There was issue with recursive calls (might be gone)

class ChessMoveLogic {
    using enum ChessPieceType;
    using possibleMovesFunc = std::function<PieceTiles(const ChessTile &fromTile)>;

  public:
    explicit ChessMoveLogic(const ChessBoard &board);

    const ChessBoard &getChessBoard() const { return board; }

    // finds possible moves
    PieceTiles getPossibleMoves(const ChessTile &fromTile) const;
    PieceTiles getPossibleLegalMoves(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesNoCastling(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesUncached(const ChessTile &fromTile) const;
    PieceTiles getAllPossibleMoves(bool white) const;
    PieceTiles getAllPossibleMovesPiece(bool white, ChessPieceType piece) const;
    PieceMoves getAllLegalMoves(bool white) const;
    PieceMoves getAllLegalMoves() const { return getAllLegalMoves(board.isWhitesTurn()); }
    std::map<ChessTile, PieceTiles> getAllPossibleMovesMap(bool white) const;

    // manipulate a possible moves list
    void filterPossibleMovesForChecks(const ChessTile &fromTile, PieceTiles &possibleMoves) const;
    static bool addIfPossibleMove(const ChessTile &fromTile, const ChessTile &toTile, PieceTiles &possibleMoves);

    // checks for things on the board
    bool isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile) const;
    bool isTileAttacked(bool white, const PieceTiles &tilesToCheck) const;
    bool isKingChecked(bool white) const;
    bool isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile) const;
    bool isKingCheckmate(bool white) const;
    bool isDraw(bool white) const;
    bool isThreefoldRepetition() const;
    bool isDeadPosition() const;

    void resetCache() { possibleMovesCache.clear(); }

    static bool isTileFree(const PieceTiles &tilesToCheck) {
        return !std::ranges::any_of(tilesToCheck, [](const ChessTile *tile) { return !tile->hasPiece(NONE); });
    }

  private:
    const ChessBoard &board;

    mutable std::map<const ChessTile *, PieceTiles> possibleMovesCache;

    // helpers for find possible moves
    PieceTiles getPossibleMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func) const;
    PieceTiles getPossibleMovesPawn(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesBishop(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesKnight(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesRook(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesQueen(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesKing(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesKingSingle(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesCastling(const ChessTile &fromTile) const;
    PieceTiles getPossibleMovesByDirection(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;
    PieceTiles getPossibleMovesByDirectionSingle(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;

    PieceTiles getLeftCastleTiles(int x, int y) const { return {&board.getTileAt(x - 1, y), &board.getTileAt(x - 2, y)}; }
    PieceTiles getRightCastleTiles(int x, int y) const { return {&board.getTileAt(x + 1, y), &board.getTileAt(x + 2, y)}; }
};

#endif // CHESSMOVELOGIC_H
