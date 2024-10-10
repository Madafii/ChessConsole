//
// Created by fpittermann on 10/5/24.
//

#include "ChessInstance.h"
#include <iostream>

ChessInstance::ChessInstance() {
    ChessBoard *chessBoard = new ChessBoard();
    _chessBoard = *chessBoard;

    run();
}

void ChessInstance::run() {
    std::string input;
    while(true) {
        std::cin >> input;
        if (input == "quit") break;
        _chessBoard.updateBoard();
    }
}

