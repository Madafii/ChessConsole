#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "ChessPiece.h"
#include "ChessTile.h"
#include <memory>
#include <optional>
#include <vector>

class ChessBoardDraw;
class ChessMoveLogic;

constexpr int boardWidth = 8;
constexpr int boardHeight = 8;

using Pieces = std::vector<ChessTile *>;
using PiecePair = std::optional<std::pair<ChessTile *, ChessTile *>>;
enum class GameState { WON, DRAW, IN_PROGRESS };

class ChessBoard {
  public:
    explicit ChessBoard(bool doAfterMoveChecks = true);
    ~ChessBoard() = default;

    ChessBoard(const ChessBoard &otherBoard);
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
    [[nodiscard]] std::vector<std::string> getGameHistory() const { return gameHistory; }
    [[nodiscard]] static std::string getMoveName(const ChessTile *fromTile, const ChessTile *toTile);
    [[nodiscard]] std::vector<ChessTile *> getBoard() const;

    Pieces getAllPossibleMovesPiece(bool white, ChessPieceType piece);
    // get tiles
    Pieces getAllPiecesFor(bool white, ChessPieceType piece) const;
    Pieces getAllWhiteTiles() const;
    Pieces getAllBlackTiles() const;
    ChessTile *getTileAt(std::string_view pos) const;
    ChessTile *getTileAt(int x, int y) const;
    ChessTile *getTileAt(int pos) const;

    static void mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge);

  private:
    std::vector<std::unique_ptr<ChessTile>> board;
    std::unordered_map<const ChessTile *, Pieces> possibleMovesCache;
    bool whitesTurn = true;
    std::pair<bool, bool> whiteRookMoved = {false, false};
    std::pair<bool, bool> blackRookMoved = {false, false};
    std::vector<std::string> gameHistory;
    bool markTurnForEnPassant = false;
    std::pair<int, int> doublePawnMoveAt = std::make_pair(-1, -1); // says in what column a pawn move with two steps happened
    bool enPassantPossibleLastMove = false;
    int movesSinceLastCapture = 0;

    // options for performance
    bool doAfterMoveChecks{false};

    // moves
    void move(ChessTile *fromTile, ChessTile *toTile);
    void movePawn(const ChessTile *fromTile, const ChessTile *toTile);
    void moveKing(const ChessTile *fromTile, const ChessTile *toTile);
    void moveRook(const ChessTile *fromTile);

    void pawnWon(ChessTile *pawnTile, char pawnToPiece = '0') const;

    GameState afterMoveChecks(ChessTile *toTile, char pawnToPiece = '0');

    PiecePair getMoveTilesFromInput(const std::string &input) const;

    friend ChessBoardDraw;
    friend ChessMoveLogic;
};

#endif // CHESSBOARD_H
