#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <functional>
#include <map>

class ChessMoveLogic {
    using enum ChessPieceType;
    using possibleMovesFunc = std::function<Pieces(const ChessTile &fromTile)>;

  public:
    explicit ChessMoveLogic(const ChessBoard &board);

    ChessBoard getChessBoard() const { return board; }

    // finds possible moves
    Pieces getPossibleMoves(const ChessTile &fromTile);
    Pieces getPossibleMovesUncached(const ChessTile &fromTile);
    Pieces getAllPossibleMoves(bool white);

    // manipulate a possible moves list
    void filterPossibleMovesForChecks(const ChessTile &fromTile, Pieces &possibleMoves);
    bool addIfPossibleMove(const ChessTile &fromTile, const ChessTile &toTile, Pieces &possibleMoves) const;

    // checks for things on the board
    bool isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile);
    bool isTileAttacked(bool white, const Pieces &tilesToCheck);
    bool isTileFree(const Pieces &tilesToCheck) const;
    bool isKingChecked(bool white, bool cached = true);
    bool isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile);
    bool isKingCheckmate();
    bool isDraw();
    bool isThreefoldRepetition() const;
    bool isDeadPosition() const;

  private:
    const ChessBoard &board;

    std::map<const ChessTile*, Pieces> possibleMovesCache;

    // helpers for find possible moves
    Pieces getPossibleMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func);
    Pieces getPossibleMovesPawn(const ChessTile &fromTile);
    Pieces getPossibleMovesBishop(const ChessTile &fromTile);
    Pieces getPossibleMovesKnight(const ChessTile &fromTile);
    Pieces getPossibleMovesRook(const ChessTile &fromTile);
    Pieces getPossibleMovesQueen(const ChessTile &fromTile);
    Pieces getPossibleMovesKing(const ChessTile &fromTile);
    Pieces getPossibleMovesKingSingle(const ChessTile &fromTile, bool cache = true);
    Pieces getPossibleMovesCastling(const ChessTile &fromTile);
    Pieces getPossibleMovesByDirection(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;
    Pieces getPossibleMovesByDirectionSingle(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const;
};

#endif // CHESSMOVELOGIC_H
