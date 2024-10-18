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
    _chessBoard.updateBoard();
    while(true) {
        std::cin >> input;
        if (input == "quit") break;
        else {
            _chessBoard.handleMoveInput(input);
        }
        _chessBoard.updateBoard();
        if(_chessBoard.isKingCheckmate()) {
            break;
        }
    }
}

