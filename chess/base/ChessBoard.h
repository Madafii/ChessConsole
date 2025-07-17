#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "ChessTile.h"
#include <array>
#include <optional>
#include <vector>

class ChessBoardDraw;
class ChessMoveLogic;

static constexpr int boardWidth = 8;
static constexpr int boardHeight = 8;
static constexpr int boardSize = boardWidth * boardHeight;

using Pieces = std::vector<const ChessTile *>;
using PiecePair = std::optional<std::pair<ChessTile *, ChessTile *>>;

enum class GameState { WON, DRAW, IN_PROGRESS };

class ChessBoard {
  public:
    explicit ChessBoard() { initBoard(); };
    ~ChessBoard() = default;

    // create the board
    void initBoard();

    // basic checks
    bool isWhitesTurn() const { return whitesTurn; }
    bool isWhiteCastlePossible() const { return !whiteRookMoved.first || !whiteRookMoved.second; }
    bool isBlackCastlePossible() const { return !blackRookMoved.first || !blackRookMoved.second; }
    bool isEnPassantPossible() const { return enPassantPossibleLastMove; }

    // board data getters
    std::array<ChessTile, boardSize> getBoard() const { return board; }
    std::vector<std::string> getGameHistory() const { return gameHistory; }
    int getMovesSinceLastCapture() const { return movesSinceLastCapture; }
    bool getEnPassantMarker() const { return markTurnForEnPassant; }
    auto getWhiteRookMoved() const { return whiteRookMoved; }
    auto getBlackRookMoved() const { return blackRookMoved; }
    auto getLastDoublePawnMove() const { return doublePawnMoveAt; }

    // get tiles
    Pieces getAllPiecesFor(bool white, ChessPieceType piece) const;
    Pieces getAllWhiteTiles() const;
    Pieces getAllBlackTiles() const;
    Pieces getWhitePieceType(ChessPieceType piece) const;
    Pieces getBlackPieceType(ChessPieceType piece) const;

    // setters
    void setTurn(const bool white) { whitesTurn = white; }
    void setEnPassantPossible(const bool possible) { enPassantPossibleLastMove = possible; }
    void setEnPassantMarker(const bool white) { markTurnForEnPassant = white; }
    void setLastDoublePawnMove(const std::pair<int, int> &pawnPos) { doublePawnMoveAt = pawnPos; }
    void resetLastDoublePawnMove() { doublePawnMoveAt = {-1, -1}; }
    void addToGameHistory(const std::string &boardStr) { gameHistory.push_back(boardStr); }
    void incrementMoves() { movesSinceLastCapture++; }

    // board pos validators
    static bool validTilePos(std::string_view pos);
    static bool validTilePos(const int x, const int y) { return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight; }
    static bool validTilePos(const int pos) { return pos >= 0 && pos < boardWidth * boardHeight; }

    // board getters
    const ChessTile &getTileAt(const int x, const int y) const { return board[y * boardWidth + x]; }
    const ChessTile &getTileAt(const int pos) const { return board[pos]; }
    const ChessTile &getTileAt(const std::string_view pos) const { return getTileAt(ChessTile::mapXtoInt.at(pos[0]), pos[1] - '0'); }

    ChessTile &getTileAt(const int x, const int y) { return board[y * boardWidth + x]; }
    ChessTile &getTileAt(const int pos) { return board[pos]; }
    ChessTile &getTileAt(const std::string_view pos) { return getTileAt(ChessTile::mapXtoInt.at(pos[0]), pos[1] - '0'); }

    void move(ChessTile &fromTile, ChessTile &toTile, char pawnToPiece = '0');
    void endMove();

    std::string getStringFromBoard() const;

    static std::string getMoveName(const ChessTile &fromTile, const ChessTile &toTile);
    static void mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge);

  private:
    // data of the board
    std::array<ChessTile, boardSize> board;
    std::vector<std::string> gameHistory;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    std::pair<int, int> doublePawnMoveAt = std::make_pair(-1, -1); // says in what column a pawn move with two steps happened
    bool whitesTurn = true;
    bool enPassantPossibleLastMove = false;
    bool markTurnForEnPassant = false;
    int movesSinceLastCapture = 0;


    // moves
    void movePawn(const ChessTile &fromTile, const ChessTile &toTile);
    void moveKing(const ChessTile &fromTile, const ChessTile &toTile);
    void moveRook(const ChessTile &fromTile);
    static void pawnWon(ChessTile &pawnTile, char pawnToPiece);


    friend ChessBoardDraw;
};

#endif // CHESSBOARD_H
