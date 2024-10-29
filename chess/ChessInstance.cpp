//
// Created by fpittermann on 10/5/24.
//

#include "ChessInstance.h"
#include <iostream>

ChessInstance::ChessInstance() {
    _chessBoard = *new ChessBoard();

    run();
}

void ChessInstance::run() {
    std::string input;
    while(true) {
        std::cin >> input;
        if (input == "quit") break;
        const GameState game_state = _chessBoard.handleMoveInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

