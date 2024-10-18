//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "ChessTile.h"
#include "ChessPiece.h"
#include <vector>
#include <map>

constexpr int boardWidth = 8;
constexpr int boardHeight = 8;

using Pieces = std::vector<ChessTile*>;
class ChessBoard {
public:
    explicit ChessBoard();

    void initBoard();
    void updateBoard() const;
    void handleMoveInput(const std::string &input);

    bool isKingCheckmate();
private:
    void move(ChessTile *fromTile, ChessTile *toTile);
    void mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge) const;

    Pieces getPossibleMoves(const ChessTile * fromTile);
    Pieces getPossibleMovesPawn(const ChessTile *fromTile) const;
    Pieces getPossibleMovesBishop(const ChessTile *fromTile) const;
    Pieces getPossibleMovesRook(const ChessTile *fromTile) const;
    Pieces getPossibleMovesKnight(const ChessTile *fromTile) const;
    Pieces getPossibleMovesQueen(const ChessTile *fromTile) const;
    Pieces getPossibleMovesKing(const ChessTile *fromTile) const;
    Pieces getPossibleMovesByDirection(const ChessTile *fromTile,
                                                         const std::vector<std::pair<int, int>> &directions) const;
    Pieces getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                      const std::vector<std::pair<int, int>> &directions) const;
    Pieces getAllWhiteTiles() const;
    Pieces getAllBlackTiles() const;
    void filterPossibleMovesForChecks(const ChessTile *fromTile, Pieces &possibleMoves);

    bool isPossibleMove(bool fromTileWhite, ChessTile *toTile, Pieces &possibleMoves,
                        bool isPawnSpecialMove = false) const;
    bool isKingChecked(bool white);

    ChessTile *getTileAt(const std::string &pos) const;
    ChessTile *getTileAt(int x,int y) const;

    Pieces board;
    bool whitesTurn = true;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    int doublePawnMoveAt = -1; // says in what column a pawn move with two steps happened
    std::map<char, int> mapXtoInt;
};

#endif //CHESSBOARD_H
