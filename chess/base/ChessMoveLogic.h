#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <functional>

using possibleMovesFunc = std::function<Pieces(const ChessTile &fromTile)>;

class ChessMoveLogic {
    using enum ChessPieceType;
  public:
    explicit ChessMoveLogic(const ChessBoard &board);

    // finds possible moves
    Pieces getPossibleMoves(const ChessTile &fromTile) const;
    Pieces getPossibleMovesUncached(const ChessTile &fromTile) const;
    Pieces getAllPossibleMoves(bool white) const;

    // manipulate a possible moves list
    void filterPossibleMovesForChecks(const ChessTile &fromTile, Pieces &possibleMoves) const;
    bool addIfPossibleMove(const ChessTile &fromTile, const ChessTile &toTile, Pieces &possibleMoves) const;

    // checks for things on the board
    bool isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile) const;
    bool isTileAttacked(bool white, const Pieces &tilesToCheck) const;
    bool isTileFree(const Pieces &tilesToCheck) const;
    bool isKingChecked(bool white, bool cached = true) const;
    bool isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile) const;
    bool isKingCheckmate() const;
    bool isDraw() const;
    bool isThreefoldRepetition() const;
    bool isDeadPosition() const;

  private:
    const ChessBoard &_board;

    // helpers for find possible moves
    Pieces getPossibleMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func) const;
    Pieces getPossibleMovesPawn(const ChessTile &fromTile) const;
    Pieces getPossibleMovesBishop(const ChessTile &fromTile) const;
    Pieces getPossibleMovesKnight(const ChessTile &fromTile) const;
    Pieces getPossibleMovesRook(const ChessTile &fromTile) const;
    Pieces getPossibleMovesQueen(const ChessTile &fromTile) const;
    Pieces getPossibleMovesKing(const ChessTile &fromTile) const;
    Pieces getPossibleMovesKingSingle(const ChessTile &fromTile, bool cache = true) const;
    Pieces getPossibleMovesCastling(const ChessTile &fromTile) const;
    Pieces getPossibleMovesByDirection(const ChessTile &fromTile,
                                       const std::vector<std::pair<int, int>> &directions) const;
    Pieces getPossibleMovesByDirectionSingle(const ChessTile &fromTile,
                                             const std::vector<std::pair<int, int>> &directions) const;
};

#endif // CHESSMOVELOGIC_H
