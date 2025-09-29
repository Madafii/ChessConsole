#include "ChessPeepo.h"
#include "ChessBoard.h"
#include "ChessDatabaseInterface.h"
#include "ChessInterface.h"
#include "ChessLinkedListMoves.h"
#include "ChessMoveLogic.h"
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
        inputMove = getRandomInputMove(chessInterface);
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

std::string ChessPeepo::getRandomInputMove(ChessInterface &chessI) {
    // random seeder thing
    std::random_device rd;
    std::mt19937 gen(rd());

    const ChessBoard &board = chessI.getChessBoard();
    ChessMoveLogic &logic = chessI.getChessMoveLogic();

    // get respective color pieces
    Pieces allPieces = board.isWhitesTurn() ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    std::uniform_int_distribution<> distrFrom(0, static_cast<int>(allPieces.size()) - 1);

    // find a random move
    Pieces possMoves;
    std::string input;
    // find a move that has at least one possible move
    while (true) {
        const int rndFromPiece = distrFrom(gen);
        const ChessTile *fromTile = allPieces.at(rndFromPiece);
        possMoves = logic.getPossibleMovesUncached(*fromTile);
        logic.filterPossibleMovesForChecks(*fromTile, possMoves);
        if (!possMoves.empty()) {
            input += fromTile->getMove() + ":";
            break;
        }
    }
    std::uniform_int_distribution<> distrTo(0, static_cast<int>(possMoves.size()) - 1);
    const ChessTile *toTile = possMoves.at(distrTo(gen));
    input += toTile->getMove();
    return input;
}

