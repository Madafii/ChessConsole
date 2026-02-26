#ifndef CHESSMOVELOGIC_H
#define CHESSMOVELOGIC_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <map>

class ChessMoveLogic {
  public:
    using enum ChessPieceType;
    using possibleMovesFunc = std::function<PieceTiles(const ChessTile &fromTile)>;
    template <size_t N>
    using directionArray = std::array<std::pair<int8_t, int8_t>, N>;

    static constexpr directionArray<4> directionsDiagonal{std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    static constexpr directionArray<4> directionsStraight{std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    static constexpr directionArray<8> directionsAll{std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1),
                                                     std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    static constexpr directionArray<8> directionsKnight{std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                                        std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};

    explicit ChessMoveLogic(const ChessBoard &board);

    const ChessBoard &getChessBoard() const { return _board; }

    // finds possible moves
    PieceTiles getLegalMoves(const ChessTile &fromTile) const;
    PieceTiles getMovesNoCastling(const ChessTile &fromTile) const;
    PieceTiles getMoves(const ChessTile &fromTile) const;
    PieceTiles getAllLegalToTiles(bool white) const;
    PieceTiles getAllLegalMovesPiece(bool white, ChessPieceType piece) const;
    PieceMoves getAllLegalMoves(bool white) const;
    PieceMoves getAllLegalMoves() const { return getAllLegalMoves(_board.isWhitesTurn()); }
    std::map<ChessTile, PieceTiles> getAllLegalMovesMap(bool white) const;

    // manipulate a possible moves list
    void filterLegalMoves(const ChessTile &fromTile, PieceTiles &possibleMoves) const;
    static bool addIfLegalMove(const ChessTile &fromTile, const ChessTile &toTile, PieceTiles &possibleMoves);

    // checks for things on the board
    bool isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile) const;
    bool isTileAttacked(bool white, const PieceTiles &tilesToCheck) const;
    bool isKingChecked(bool white) const;
    bool isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile) const;
    bool isKingCheckmate(bool white) const;
    bool isDraw(bool white) const;
    bool isThreefoldRepetition() const;
    bool isDeadPosition() const;

    void resetCache() { _possibleMovesCache.clear(); }

    static bool isTileFree(const PieceTiles &tilesToCheck) {
        return !std::ranges::any_of(tilesToCheck, [](const ChessTile *tile) { return tile->hasPiece(); });
    }
    static bool isCastling(const ChessTile &fromTile, const ChessTile &toTile) {
        return fromTile.hasPiece(KING) && std::abs(fromTile.getX() - toTile.getX()) == 2;
    }

  private:
    const ChessBoard &_board;

    mutable std::map<const ChessTile *, PieceTiles> _possibleMovesCache;

    // helpers for find possible moves
    PieceTiles getMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func) const;
    PieceTiles getMovesPawn(const ChessTile &fromTile) const;
    PieceTiles getMovesBishop(const ChessTile &fromTile) const { return getMovesByDirection(fromTile, directionsDiagonal); }
    PieceTiles getMovesKnight(const ChessTile &fromTile) const { return getMovesByDirectionSingle(fromTile, directionsKnight); }
    PieceTiles getMovesRook(const ChessTile &fromTile) const { return getMovesByDirection(fromTile, directionsStraight); }
    PieceTiles getMovesQueen(const ChessTile &fromTile) const { return getMovesByDirection(fromTile, directionsAll); }
    PieceTiles getMovesKing(const ChessTile &fromTile) const;
    PieceTiles getMovesKingSingle(const ChessTile &fromTile) const { return getMovesByDirectionSingle(fromTile, directionsAll); }
    PieceTiles getMovesCastling(const ChessTile &fromTile) const;
    template <size_t N>
    PieceTiles getMovesByDirection(const ChessTile &fromTile, const directionArray<N> &directions) const;
    template <size_t N>
    PieceTiles getMovesByDirectionSingle(const ChessTile &fromTile, const directionArray<N> &directions) const;

    PieceTiles getLeftCastleTiles(int x, int y) const { return {&_board.getTileAt(x - 1, y), &_board.getTileAt(x - 2, y)}; }
    PieceTiles getRightCastleTiles(int x, int y) const { return {&_board.getTileAt(x + 1, y), &_board.getTileAt(x + 2, y)}; }
};

#endif // CHESSMOVELOGIC_H
