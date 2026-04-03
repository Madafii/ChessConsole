#ifndef CHESSINTERFACE_H
#define CHESSINTERFACE_H

#include <ChessBoard.h>
#include <ChessBoardDraw.h>
#include <ChessMoveLogic.h>

class ChessInterface {
  public:
    explicit ChessInterface();
    ChessInterface(const ChessBoard &board);
    ChessInterface(const ChessInterface &other) = default;
    ChessInterface &operator=(const ChessInterface &other);

    // handle inputs to the game
    std::optional<GameState> handleInput(std::string_view input);
    std::optional<GameState> handleMoveInput(std::string_view input);
    void handleMoveInputNoChecks(std::string_view input);
    void handleMoveInputNoChecks(const ChessTile &fromTile, const ChessTile &toTile, char pawnPromotion = '0');

    // handle single tile inputs
    std::optional<PieceTiles> getPossibleMovesFromTile(std::string_view input);
    std::optional<ChessTile> handleToInput(std::string_view input, const PieceTiles &possMoves) const;

    // getters
    ChessBoard &getChessBoard() { return chessBoard; }
    ChessMoveLogic &getChessMoveLogic() { return chessLogic; }

    GameState checkGameState();

  private:
    ChessBoard chessBoard = ChessBoard();
    ChessMoveLogic chessLogic = ChessMoveLogic(chessBoard);
    ChessBoardDraw chessDraw = ChessBoardDraw();

    // options for performance
    bool doGameStateCheck = true;

    PiecePair getMoveTilesFromInput(std::string_view input);
    std::optional<ChessTile> getMoveTileFromInput(std::string_view input) const;

    // some checks with messages
    bool checkInputLength(std::string_view input, size_t length) const;
    bool checkValidTilePos(const std::vector<std::string_view> &moves) const;
    bool checkFromTile(const ChessTile &tile) const;

    // splits input into from:to and pawnWonTo
    static std::pair<std::string, char> splitMoveInput(std::string_view input) {
        return {std::string(input.substr(0, 5)), (input.length() == 7 ? input[6] : '0')};
    }
};

#endif
