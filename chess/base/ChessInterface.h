#ifndef CHESSINTERFACE_H
#define CHESSINTERFACE_H

#include <ChessBoard.h>
#include <ChessBoardDraw.h>
#include <ChessMoveLogic.h>

class ChessInterface {
  public:
    explicit ChessInterface();

    const ChessBoard &getChessBoard() const { return chessBoard; }
    ChessMoveLogic &getChessMoveLogic() { return chessLogic; }

    // handle inputs to the game
    GameState handleInput(std::string_view input);
    GameState handleMoveInput(std::string_view input);
    void handleMoveInputNoChecks(std::string_view input, bool enPassant);

    // some getters of pieces
    Pieces getAllPossibleMovesPiece(bool white, ChessPieceType piece);

    GameState afterMoveChecks(ChessTile &toTile, char pawnToPiece = '0');
    PiecePair getMoveTilesFromInput(std::string_view input);

    void pawnWon(ChessTile &pawnTile, char pawnToPiece = '0') const;

  private:
    ChessBoard chessBoard;
    ChessBoardDraw chessDraw;
    ChessMoveLogic chessLogic;

    // options for performance
    bool doAfterMoveChecks = true;
};

#endif
