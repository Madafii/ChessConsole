//
// Created by fpittermann on 10/5/24.
//

#include "ChessInstance.h"
#include <iostream>
#include <random>

ChessInstance::ChessInstance() {
    _chessBoard = *new ChessBoard();

    // run();
    runRandom();
}

void ChessInstance::run() {
    std::string input;
    while (true) {
        std::cin >> input;
        if (input == "quit")
            break;
        const GameState game_state = _chessBoard.handleMoveInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}
void ChessInstance::runRandom() {
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    while (true) {
        Pieces allPieces = _chessBoard.isWhitesTurn() ? _chessBoard.getAllWhiteTiles() : _chessBoard.getAllBlackTiles();
        std::uniform_int_distribution<> distrFrom(0, allPieces.size() - 1);
        std::string input;
        Pieces possMoves;
        while (true) {
            const int rndFromPiece = distrFrom(gen);
            const ChessTile *fromTile = allPieces.at(rndFromPiece);
            possMoves = _chessBoard.getPossibleMoves(fromTile);
            if (possMoves.size() != 0) {
                input += fromTile->getMove() + ":";
                break;
            }
        }
        std::uniform_int_distribution<> distrTo(0, possMoves.size() - 1);
        const int rndToPiece = distrTo(gen);
        const ChessTile *toTile = possMoves.at(rndToPiece);
        input += toTile->getMove();
        const GameState game_state = _chessBoard.handleMoveInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}
