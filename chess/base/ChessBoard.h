//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "ChessPiece.h"
#include "ChessTile.h"
#include <optional>
#include <vector>

constexpr int boardWidth = 8;
constexpr int boardHeight = 8;

using Pieces = std::vector<ChessTile *>;
using PiecePair = std::optional<std::pair<ChessTile *, ChessTile *>>;
enum class GameState
{
    WON,
    DRAW,
    IN_PROGRESS
};

class ChessBoard
{
  public:
    explicit ChessBoard();
    ~ChessBoard();

    void initBoard();
    void updateBoard() const;
    GameState handleInput(const std::string &input);
    GameState handleMoveInput(std::string input);

    std::string getStringFromBoard();

    bool isKingCheckmate();
    bool isDraw();
    bool isWhitesTurn();

    Pieces getAllPossibleMoves(bool white);
    Pieces getAllPossibleMovesPiece(bool white, ChessPieceType piece);
    Pieces getAllPiecesFor(bool white, ChessPieceType piece) const;
    Pieces getAllWhiteTiles() const;
    Pieces getAllBlackTiles() const;

    Pieces getPossibleMoves(const ChessTile *fromTile);
    void filterPossibleMovesForChecks(const ChessTile *fromTile, Pieces &possibleMoves);

  private:
    void move(ChessTile *fromTile, ChessTile *toTile);
    void movePawn(const ChessTile *fromTile, const ChessTile *toTile);
    void moveKing(const ChessTile *fromTile, const ChessTile *toTile);
    void moveRook(const ChessTile *fromTile);
    void pawnWon(ChessTile *pawnTile, char pawnToPiece = '0') const;

    GameState afterMoveChecks(ChessTile *toTile, char pawnToPiece = '0');

    PiecePair getMoveTilesFromInput(const std::string &input) const;

    Pieces getPossibleMovesPawn(const ChessTile *fromTile);
    Pieces getPossibleMovesBishop(const ChessTile *fromTile);
    Pieces getPossibleMovesRook(const ChessTile *fromTile);
    Pieces getPossibleMovesKnight(const ChessTile *fromTile);
    Pieces getPossibleMovesQueen(const ChessTile *fromTile);
    Pieces getPossibleMovesKing(const ChessTile *fromTile, bool castling = true);
    Pieces getPossibleMovesByDirection(const ChessTile *fromTile, const std::vector<std::pair<int, int>> &directions);
    Pieces getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                             const std::vector<std::pair<int, int>> &directions);
    Pieces getPossibleMovesCastling(const ChessTile *fromTile);

    bool isInputMovePossible(const ChessTile *fromTile, const ChessTile *toTile);
    bool isPossibleMove(const ChessTile *fromTile, ChessTile *toTile, Pieces &possibleMoves);
    bool isKingChecked(bool white);
    bool isKingChecked(const ChessTile *fromTile, ChessTile *toTile);
    bool isTileAttackedAndFree(bool white, const Pieces &tilesToCheck);
    bool isThreefoldRepetition();

    ChessTile *getTileAt(const std::string &pos) const;
    ChessTile *getTileAt(int x, int y) const;

    static void mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge);

    Pieces board;
    bool whitesTurn = true;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    std::vector<std::string> gameHistory;
    bool markTurnForEnPassant = false;
    std::pair<int, int> doublePawnMoveAt = std::make_pair(-1, -1); // says in what column a pawn move with two steps happened
    bool enPassantPossibleLastMove = false;
    int movesSinceLastCapture = 0;
};

inline void ChessBoard::mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge)
{
    possibleMoves.insert(possibleMoves.end(), possibleMovesMerge.begin(), possibleMovesMerge.end());
}

#endif // CHESSBOARD_H
