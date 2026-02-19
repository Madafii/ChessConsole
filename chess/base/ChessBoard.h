#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "ChessPiece.h"
#include "ChessTile.h"

#include <array>
#include <optional>
#include <vector>

class ChessBoardDraw;
class ChessMoveLogic;

static constexpr int boardWidth = 8;
static constexpr int boardHeight = 8;
static constexpr int boardSize = boardWidth * boardHeight;

using PieceTiles = std::vector<const ChessTile *>;
using PiecePair = std::optional<std::pair<ChessTile *, ChessTile *>>;
using PiecePairC = std::pair<const ChessTile *, const ChessTile *>;
using PieceMoves = std::vector<std::pair<const ChessTile *, const ChessTile *>>;

enum class GameState { Won, Draw, InProgress };
enum class CastleSide { Left, Right, Both, Any };

class ChessBoard {
  public:
    ChessBoard() { initBoard(); };

    // create the board
    void initBoard();

    // board data getters
    std::array<ChessTile, boardSize> getBoard() const { return board; }
    std::vector<std::string> getGameHistory() const { return gameHistory; }
    int getMovesSinceLastCapture() const { return movesSinceLastCapture; }
    bool getEnPassantMarker() const { return markTurnForEnPassant; }
    auto getWhiteRookMoved() const -> std::pair<bool, bool> { return whiteRookMoved; }
    auto getBlackRookMoved() const -> std::pair<bool, bool> { return blackRookMoved; }
    auto getLastDoublePawnMove() const -> std::pair<int, int> { return doublePawnMoveAt; }

    // get tiles
    PieceTiles getAllPiecesFor(bool white, ChessPieceType piece) const;
    PieceTiles getAllTiles(bool white) const { return white ? getAllWhiteTiles() : getAllBlackTiles(); }
    PieceTiles getAllTiles() const { return getAllTiles(whitesTurn); }
    PieceTiles getAllWhiteTiles() const;
    PieceTiles getAllBlackTiles() const;
    PieceTiles getPieceType(bool white, ChessPieceType piece) const;
    PieceTiles getWhitePieceType(ChessPieceType piece) const;
    PieceTiles getBlackPieceType(ChessPieceType piece) const;

    // setters
    void setTurn(const bool white) { whitesTurn = white; }
    void toggleTurn() { whitesTurn = !whitesTurn; }
    void setEnPassantPossible(const bool possible) { enPassantPossibleLastMove = possible; }
    void setEnPassantMarker(const bool white) { markTurnForEnPassant = white; }
    void setLastDoublePawnMove(const std::pair<int, int> &pawnPos) { doublePawnMoveAt = pawnPos; }
    void resetLastDoublePawnMove() { doublePawnMoveAt = {-1, -1}; }
    void pushMoveToHistory() { gameHistory.push_back(getStringFromBoard()); }
    void incrementMoves() { movesSinceLastCapture++; }

    // basic checks
    bool isWhitesTurn() const { return whitesTurn; }
    bool isCastlePossible(CastleSide side, bool white) const { return white ? isWhiteCastlePossible(side) : isBlackCastlePossible(side); }
    bool isWhiteCastlePossible(CastleSide side) const {
        return (side == CastleSide::Left && !whiteRookMoved.first) || (side == CastleSide::Right && !whiteRookMoved.second) ||
               (side == CastleSide::Both && !whiteRookMoved.first && !whiteRookMoved.second) ||
               (side == CastleSide::Any || !whiteRookMoved.first || !whiteRookMoved.second);
    }
    bool isBlackCastlePossible(CastleSide side) const {
        return (side == CastleSide::Left && !blackRookMoved.first) || (side == CastleSide::Right && !blackRookMoved.second) ||
               (side == CastleSide::Both && !blackRookMoved.first && !blackRookMoved.second) ||
               (side == CastleSide::Any || !blackRookMoved.first || !blackRookMoved.second);
    }
    bool isEnPassantPossible() const { return enPassantPossibleLastMove; }

    // board pos validators
    static bool validTilePos(std::string_view pos);
    static bool validTilePos(const int x, const int y) { return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight; }
    static bool validTilePos(const int pos) { return pos >= 0 && pos < boardWidth * boardHeight; }

    // board getters. direct access, so check valid pos first
    const ChessTile &getTileAt(const int x, const int y) const { return board[y * boardWidth + x]; }
    const ChessTile &getTileAt(std::pair<int, int> pos) const { return board[pos.second * boardWidth + pos.first]; }
    const ChessTile &getTileAt(const int pos) const { return board[pos]; }
    const ChessTile &getTileAt(const std::string_view pos) const { return getTileAt(ChessTile::getPos(pos[0], pos[1])); }

    ChessTile &getTileAt(const int x, const int y) { return board[y * boardWidth + x]; }
    ChessTile &getTileAt(const std::pair<int, int> &pos) { return board[pos.second * boardWidth + pos.first]; }
    ChessTile &getTileAt(const int pos) { return board[pos]; }
    ChessTile &getTileAt(const std::string_view pos) { return getTileAt(ChessTile::getPos(pos[0], pos[1])); }

    // TODO: might want to change this as it contains logic -> so split to logic?
    void move(ChessTile &fromTile, ChessTile &toTile, char pawnToPiece = '0');
    void endMove();

    std::string getStringFromBoard() const;

    static std::string getMoveName(const ChessTile &fromTile, const ChessTile &toTile);
    static void mergePossVec(PieceTiles &possibleMoves, PieceTiles possibleMovesMerge);

  private:
    // data of the board
    std::array<ChessTile, boardSize> board;
    std::vector<std::string> gameHistory = {};
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

    static bool isPawnWinCondition(const ChessTile &toTile) {
        return toTile.hasPiece(ChessPieceType::PAWN) && (toTile.getY() == 0 || toTile.getY() == 7);
    }

    friend ChessBoardDraw;
};

#endif // CHESSBOARD_H
