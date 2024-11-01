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
using PiecePair = std::optional<std::pair<ChessTile*, ChessTile*>>;
enum class GameState {
    WON,
    DRAW,
    IN_PROGRESS
};

class ChessBoard {
public:
    explicit ChessBoard();

    void initBoard();
    void updateBoard() const;
    GameState handleMoveInput(std::string input);

    bool isKingCheckmate();
    bool isDraw();

    Pieces getAllPossibleMoves(bool white);
    Pieces getAllPossibleMovesPiece(bool white, ChessPieceType piece);
    Pieces getAllPiecesFor(bool white, ChessPieceType piece) const;

    // move to private again later is just for testing
    Pieces getPossibleMoves(const ChessTile * fromTile);
private:
    void move(ChessTile *fromTile, ChessTile *toTile);
    void movePawn(const ChessTile *fromTile, const ChessTile *toTile);
    void moveKing(const ChessTile *fromTile, const ChessTile *toTile);
    void moveRook(const ChessTile *fromTile);
    void mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge) const;
    void pawnWon(ChessTile *pawnTile, char pawnToPiece = '0') const;
    GameState afterMoveChecks(ChessTile *toTile, char pawnToPiece = '0');

    PiecePair getMoveTilesFromInput(const std::string &input) const;

    Pieces getPossibleMovesPawn(const ChessTile *fromTile);
    Pieces getPossibleMovesBishop(const ChessTile *fromTile);
    Pieces getPossibleMovesRook(const ChessTile *fromTile);
    Pieces getPossibleMovesKnight(const ChessTile *fromTile);
    Pieces getPossibleMovesQueen(const ChessTile *fromTile);
    Pieces getPossibleMovesKing(const ChessTile *fromTile, bool castling = true);
    Pieces getPossibleMovesByDirection(const ChessTile *fromTile,
                                                         const std::vector<std::pair<int, int>> &directions);
    Pieces getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                      const std::vector<std::pair<int, int>> &directions);
    Pieces getPossibleMovesCastling(const ChessTile *fromTile);

    Pieces getAllWhiteTiles() const;
    Pieces getAllBlackTiles() const;
    void filterPossibleMovesForChecks(const ChessTile *fromTile, Pieces &possibleMoves);

    bool isInputMovePossible(const ChessTile *fromTile, const ChessTile *toTile);
    bool isPossibleMove(const ChessTile *fromTile, ChessTile *toTile, Pieces &possibleMoves);

    bool isKingChecked(bool white);
    bool isKingChecked(const ChessTile *fromTile, ChessTile *toTile);
    bool isTileAttackedAndFree(bool white, const Pieces &tilesToCheck);

    ChessTile *getTileAt(const std::string &pos) const;
    ChessTile *getTileAt(int x,int y) const;

    Pieces board;
    bool whitesTurn = true;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    int doublePawnMoveAt = -1; // says in what column a pawn move with two steps happened
    int movesSinceLastCapture = 0;
};

#endif //CHESSBOARD_H
