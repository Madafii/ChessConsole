#include "ChessConsoleUI.h"

#include <iostream>

#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessInterface.h"
#include "ChessUI.h"

ChessConsoleUI::ChessConsoleUI(PlayerFactory playerWhite, PlayerFactory playerBlack, const ChessBoardDrawSettings &settings)
    : ChessUI(playerWhite, playerBlack), _boardDraw(ChessBoardDraw(settings)) {}

void ChessConsoleUI::start() {
    std::cout << "...started a normal game..." << std::endl;

    _boardDraw.draw(_chessInterface.getChessBoard());
    while (true) {
        auto moveWhite = _playerWhite->getNextMove();
        if (!moveWhite) break;
        if (!doMove(moveWhite.value())) break;

        auto moveBlack = _playerBlack->getNextMove();
        if (!moveBlack) break;
        if (!doMove(moveBlack.value())) break;
    }

    std::cout << "...game ended..." << std::endl;
}

bool ChessConsoleUI::doMove(std::string_view inputMove) {
    std::cout << "move made: " << inputMove << std::endl;
    _chessInterface.handleMoveInputNoChecks(inputMove);
    _boardDraw.draw(_chessInterface.getChessBoard());
    return _chessInterface.checkGameState() == GameState::InProgress;
}
