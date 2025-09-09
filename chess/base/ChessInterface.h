#ifndef CHESSINTERFACE_H
#define CHESSINTERFACE_H

#include <ChessBoard.h>
#include <ChessBoardDraw.h>
#include <ChessMoveLogic.h>

class ChessInterface {
  public:
    explicit ChessInterface();

    // handle inputs to the game
    GameState handleInput(std::string_view input);
    GameState handleMoveInput(std::string_view input);
    void handleMoveInputNoChecks(std::string_view input, bool enPassant);
    std::optional<Pieces> handleFromInput(std::string_view input);

    // getters
    const ChessBoard &getChessBoard() const { return chessBoard; }
    ChessMoveLogic &getChessMoveLogic() { return chessLogic; }

  private:
    ChessBoard chessBoard;
    ChessMoveLogic chessLogic;
    ChessBoardDraw chessDraw;

    // options for performance
    bool doGameStateCheck = true;

    PiecePair getMoveTilesFromInput(std::string_view input);

    GameState checkGameState();

    static std::pair<std::string, char> splitMoveInput(std::string_view input) {
        return {std::string(input.substr(0, 5)), (input.length() == 7 ? input[6] : '0')};
    }
};

#endif
