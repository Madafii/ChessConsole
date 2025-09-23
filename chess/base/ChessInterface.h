#ifndef CHESSINTERFACE_H
#define CHESSINTERFACE_H

#include <ChessBoard.h>
#include <ChessBoardDraw.h>
#include <ChessMoveLogic.h>

class ChessInterface {
  public:
    explicit ChessInterface();
    ChessInterface(const ChessInterface &other) = default;
    ChessInterface &operator=(const ChessInterface &other);

    // handle inputs to the game
    std::optional<GameState> handleInput(std::string_view input);
    std::optional<GameState> handleMoveInput(std::string_view input);
    std::optional<Pieces> handleFromInput(std::string_view input);
    std::optional<ChessTile> handleToInput(std::string_view input, const Pieces &possMoves);
    void handleMoveInputNoChecks(std::string_view input, bool enPassant);

    // getters
    const ChessBoard &getChessBoard() const { return chessBoard; }
    ChessMoveLogic &getChessMoveLogic() { return chessLogic; }

  private:
    ChessBoard chessBoard = ChessBoard();
    ChessMoveLogic chessLogic = ChessMoveLogic(chessBoard);
    ChessBoardDraw chessDraw = ChessBoardDraw();

    // options for performance
    bool doGameStateCheck = true;

    PiecePair getMoveTilesFromInput(std::string_view input);
    std::optional<ChessTile> getMoveTileFromInput(std::string_view input);

    GameState checkGameState();

    // some checks with messages
    bool checkInputLength(std::string_view input, size_t length); 
    bool checkValidTilePos(const std::vector<std::string_view> &moves);
    bool checkFromTile(const ChessTile &tile);

    // offset of y axis for internal usage
    static std::string getInternalInput(std::string_view input) {
        std::string subStrFrom(input);
        subStrFrom[1] = static_cast<char>(subStrFrom[1] - 1);
        return subStrFrom;
    }
    // splits input into from:to and pawnWonTo
    static std::pair<std::string, char> splitMoveInput(std::string_view input) {
        return {std::string(input.substr(0, 5)), (input.length() == 7 ? input[6] : '0')};
    }
};

#endif
