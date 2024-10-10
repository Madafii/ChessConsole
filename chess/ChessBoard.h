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

class ChessBoard {
public:
    explicit ChessBoard();

    void initBoard();
    void updateBoard() const;
    void handleMoveInput(const std::string &input) const;
    void move(ChessTile *fromTile, ChessTile *toTile) const;

    std::vector<ChessTile*> getPossibleMoves(const ChessTile * fromTile) const;
    std::vector<ChessTile*> getPossibleMovesPawn(const ChessTile *fromTile) const;
    std::vector<ChessTile*> getPossibleMovesBishop(const ChessTile *fromTile) const;
    std::vector<ChessTile*> getPossibleMovesRook(const ChessTile *fromTile) const;
    std::vector<ChessTile*> getPossibleMovesKnight(const ChessTile *fromTile) const;
    std::vector<ChessTile*> getPossibleMovesQueen(const ChessTile *fromTile) const;
    std::vector<ChessTile*> getPossibleMovesKing(const ChessTile *fromTile) const;
    std::vector<ChessTile*> getPossibleMovesByDirection(const ChessTile *fromTile,
                                                         const std::vector<std::pair<int, int>> &directions) const;
    std::vector<ChessTile*> getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                      const std::vector<std::pair<int, int>> &directions) const;

    bool isPossibleMove(bool fromTileWhite, ChessTile *toTile, std::vector<ChessTile*> &possibleMoves) const;

    ChessTile *getTileAt(const std::string &pos) const;
    ChessTile *getTileAt(int x,int y) const;

    void mergePossVec(std::vector<ChessTile*> &possibleMoves, std::vector<ChessTile*> possibleMovesMerge) const;
private:
    std::vector<ChessTile*> board;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    int doublePawnMoveAt = -1; // says in what column a pawn move with two steps happened
    std::map<char, int> mapXtoInt;
};

#endif //CHESSBOARD_H
