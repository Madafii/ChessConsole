#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "ChessPiece.h"
#include "ChessTile.h"
#include <map>
#include <optional>
#include <vector>
#include <array>

class ChessBoardDraw;
class ChessMoveLogic;

constexpr int boardWidth = 8;
constexpr int boardHeight = 8;

using Pieces = std::vector<const ChessTile *>;
using PiecePair = std::optional<std::pair<ChessTile &, ChessTile &>>;
enum class GameState { WON, DRAW, IN_PROGRESS };

class ChessBoard {
  public:
    explicit ChessBoard(bool doAfterMoveChecks = true);
    ~ChessBoard() = default;

    // ChessBoard(const ChessBoard &otherBoard);
    // breaks because of board can't be coppied no copies of unique pointers, so TODO: would be implement them
    ChessBoard &operator=(const ChessBoard &otherBoard) = delete;

    // create the board
    void initBoard();

    // handle inputs to the game
    GameState handleInput(std::string_view input);
    GameState handleMoveInput(std::string_view input);
    void handleMoveInputNoChecks(std::string_view input, bool enPassant);

    std::string getStringFromBoard();

    // basic callers
    [[nodiscard]] bool isWhitesTurn() const { return whitesTurn; }
    [[nodiscard]] bool isCastlePossible(const bool white) const {
        if ((white && (!whiteRookMoved.first || !whiteRookMoved.second)) || (!white && (!blackRookMoved.first || !blackRookMoved.second)))
            return true;
        return false;
    }

    [[nodiscard]] std::vector<std::string> getGameHistory() const { return gameHistory; }
    [[nodiscard]] std::vector<ChessTile> getBoard() const { return board; }

    [[nodiscard]] static std::string getMoveName(const ChessTile &fromTile, const ChessTile &toTile);

    [[nodiscard]] static bool hasPiece(const ChessTile &tile) { return tile.getPiece().getType() != ChessPieceType::NONE; }

    Pieces getAllPossibleMovesPiece(bool white, ChessPieceType piece) const;
    // get tiles
    Pieces getAllPiecesFor(bool white, ChessPieceType piece) const;
    Pieces getAllWhiteTiles() const;
    Pieces getAllBlackTiles() const;
    Pieces getWhitePieceType(ChessPieceType piece) const;
    Pieces getBlackPieceType(ChessPieceType piece) const;

    bool validTilePos(std::string_view pos) const;
    bool validTilePos(const int x, const int y) const {
        return x < 0 || x >= boardWidth || y < 0 || y >= boardHeight;
    }
    bool validTilePos(const int pos) const {
        return pos < 0 || pos >= boardWidth * boardHeight;
    }

    const ChessTile &getTileAt(const int x, const int y) const {
        return board[y * boardWidth + x];
    }
    const ChessTile &getTileAt(const int pos) const {
        return board[pos];
    }
    const ChessTile &getTileAt(const std::string_view pos) const {
        return getTileAt(ChessTile::mapXtoInt.at(pos[0]), pos[1] - '0');
    }

    ChessTile &getTileAt(const int x, const int y) {
        return board[y * boardWidth + x];
    }
    ChessTile &getTileAt(const int pos) {
        return board[pos];
    }
    ChessTile &getTileAt(const std::string_view pos) {
        return getTileAt(ChessTile::mapXtoInt.at(pos[0]), pos[1] - '0');
    }

    static void mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge);

  private:
    std::array<ChessTile, 64> board {};
    std::map<ChessTile, Pieces> possibleMovesCache;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    std::pair<int, int> doublePawnMoveAt = std::make_pair(-1, -1); // says in what column a pawn move with two steps happened
    std::vector<std::string> gameHistory;
    bool markTurnForEnPassant = false;
    bool whitesTurn = true;
    bool enPassantPossibleLastMove = false;
    int movesSinceLastCapture = 0;

    // options for performance
    bool doAfterMoveChecks = false;

    // moves
    void move(ChessTile &fromTile, ChessTile &toTile);
    void movePawn(const ChessTile &fromTile, const ChessTile &toTile);
    void moveKing(const ChessTile &fromTile, const ChessTile &toTile);
    void moveRook(const ChessTile &fromTile);

    void pawnWon(ChessTile &pawnTile, char pawnToPiece = '0') const;

    GameState afterMoveChecks(ChessTile &toTile, char pawnToPiece = '0');

    PiecePair getMoveTilesFromInput(const std::string &input);

    friend ChessBoardDraw;
    friend ChessMoveLogic;
};

#endif // CHESSBOARD_H
