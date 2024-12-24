#include "ChessPeepo.h"
#include "ChessBoard.h"
#include "ChessLinkedListMoves.h"
#include "ChessMoveLogic.h"
#include <iostream>
#include <memory>
#include <random>

ChessPeepo::ChessPeepo(ChessBoard &chessBoard, ChessData &chessData) : board(chessBoard), data(chessData) {}

GameState ChessPeepo::makeMostPlayedMove() {
    // get the move random or most played
    const MoveCompressed *moveHead = data.getMoves()->getMoveHead();
    std::string inputMove;
    if (moveHead == nullptr || moveHead->nexts.empty()) {
        std::cout << "no more suggestions for moves, next move will be random" << std::endl;
        inputMove = getRandomInputMove();
    } else {
        MoveCompressed *mostPlayedMove = getMostPlayedMove(moveHead->nexts);
        data.getMoves()->setMoveHead(mostPlayedMove);
        inputMove = ChessLinkedListMoves::getMoveFromData(mostPlayedMove->data);
    }
    // play the move
    // TODO: enPassant thing probaply should be checked inside ChessBoard so could remove that parameter later
    std::cout << "peepo plays the move: " << inputMove << std::endl;
    return board.handleMoveInput(inputMove);
}

void ChessPeepo::makeHighesWinRateMove() {}

void ChessPeepo::makeBestMove() {}

void ChessPeepo::makeRandomBestMove(const int &randomRange) {}

void ChessPeepo::makeRandomMove() {}

// some random checks
bool ChessPeepo::isBoardDataOnSync() {}

MoveCompressed *ChessPeepo::getMostPlayedMove(const std::vector<std::unique_ptr<MoveCompressed>> &moves) {
    MoveCompressed *mostPlayedMove = nullptr;
    u_int64_t currPlays = 0;
    for (const auto &move : moves) {
        const u_int64_t plays = playedMoves(move.get());
        if (mostPlayedMove == nullptr || plays > currPlays) {
            mostPlayedMove = move.get();
            currPlays = plays;
        }
    }
    return mostPlayedMove;
}

std::string ChessPeepo::getRandomInputMove() {
    // random seeder thing
    std::random_device rd;
    std::mt19937 gen(rd());

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
        possMoves = ChessMoveLogic::getPossibleMoves(board, fromTile);
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

inline u_int64_t ChessPeepo::playedMoves(const MoveCompressed *move) { return move->wins + move->loses + move->draws; }
