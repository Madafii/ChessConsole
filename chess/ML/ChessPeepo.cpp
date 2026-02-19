#include "ChessPeepo.h"
#include "ChessBoard.h"
#include "ChessDatabaseInterface.h"
#include "ChessInterface.h"
#include "ChessLinkedListMoves.h"
#include "ChessMoveLogic.h"
#include "ChessPiece.h"
#include <iostream>
#include <memory>
#include <random>

ChessPeepo::ChessPeepo(ChessInterface &setChessInterface, ChessData &chessData) : chessInterface(setChessInterface), data(chessData) {}

GameState ChessPeepo::makeMostPlayedMove() {
    // get the move random or most played
    const MoveCompressed *moveHead = data.getMoves()->getMoveHead();
    std::string inputMove;
    if (moveHead == nullptr || moveHead->nexts.empty()) {
        std::cout << "no more suggestions for moves, next move will be random" << std::endl;
        // TODO: deactivated. Moved to ChessPlayerRnd
        // inputMove = getRandomInputMove(chessInterface);
    } else {
        std::vector<MoveCompressed *> currNexts;
        currNexts.reserve(moveHead->nexts.size());
        for (const auto &nextMove : moveHead->nexts) {
            currNexts.push_back(nextMove.get());
        }
        MoveCompressed *mostPlayedMove = getMostPlayedMove(currNexts);
        data.getMoves()->setMoveHead(mostPlayedMove);
        inputMove = ChessLinkedListMoves::getMoveFromData(mostPlayedMove->data);
    }
    // play the move
    // TODO: enPassant thing probaply should be checked inside ChessBoard so could remove that parameter later
    std::cout << "peepo plays the move: " << inputMove << std::endl;
    return chessInterface.handleMoveInput(inputMove).value();
}

// void ChessPeepo::makeHighesWinRateMove() {}
//
// void ChessPeepo::makeBestMove() {}
//
// void ChessPeepo::makeRandomBestMove(const int &randomRange) {}
//
// void ChessPeepo::makeRandomMove() {}
//
// bool ChessPeepo::makeBestDBMove() {
//
// }

MoveCompressed *ChessPeepo::getMostPlayedMove(const std::vector<MoveCompressed*> &moves) {
    MoveCompressed *mostPlayedMove = nullptr;
    uint64_t currPlays = 0;
    for (const auto &move : moves) {
        const uint64_t plays = playedMoves(move);
        if (mostPlayedMove == nullptr || plays > currPlays) {
            mostPlayedMove = move;
            currPlays = plays;
        }
    }
    return mostPlayedMove;
}

