//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "ChessTile.h"
#include "ChessPiece.h"
#include <vector>
#include <map>
#include <optional>

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
    bool isDraw();

    Pieces getAllPossibleMoves(bool white);
private:
    void move(ChessTile *fromTile, ChessTile *toTile);
    void movePawn(const ChessTile *fromTile, const ChessTile *toTile);
    void moveKing(const ChessTile *fromTile, const ChessTile *toTile);
    void moveRook(const ChessTile *fromTile);
    void mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge) const;
    void pawnWon(ChessTile *pawnTile) const;
    void afterMoveChecks(ChessTile *toTile);

    std::optional<std::pair<ChessTile*, ChessTile*>> getMoveTilesFromInput(const std::string &input) const;

    Pieces getPossibleMoves(const ChessTile * fromTile);
    Pieces getPossibleMovesPawn(const ChessTile *fromTile) const;
    Pieces getPossibleMovesBishop(const ChessTile *fromTile) const;
    Pieces getPossibleMovesRook(const ChessTile *fromTile) const;
    Pieces getPossibleMovesKnight(const ChessTile *fromTile) const;
    Pieces getPossibleMovesQueen(const ChessTile *fromTile) const;
    Pieces getPossibleMovesKing(const ChessTile *fromTile, bool castling = true);
    Pieces getPossibleMovesByDirection(const ChessTile *fromTile,
                                                         const std::vector<std::pair<int, int>> &directions) const;
    Pieces getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                      const std::vector<std::pair<int, int>> &directions) const;
    Pieces getPossibleMovesCastling(const ChessTile *fromTile);

    Pieces getAllWhiteTiles() const;
    Pieces getAllBlackTiles() const;
    void filterPossibleMovesForChecks(const ChessTile *fromTile, Pieces &possibleMoves);

    bool isInputMovePossible(const ChessTile *fromTile, const ChessTile *toTile);
    bool isPossibleMove(bool fromTileWhite, ChessTile *toTile, Pieces &possibleMoves,
                        bool isPawnSpecialMove = false) const;
    bool isKingChecked(bool white);
    bool isTileAttackedAndFree(bool white, const Pieces &tilesToCheck);

    ChessTile *getTileAt(const std::string &pos) const;
    ChessTile *getTileAt(int x,int y) const;

    Pieces board;
    bool whitesTurn = true;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    int doublePawnMoveAt = -1; // says in what column a pawn move with two steps happened
    int movesSinceLastCapture = 0;
    std::map<char, int> mapXtoInt;
};

#endif //CHESSBOARD_H
