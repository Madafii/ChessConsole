#include "ChessData.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <chrono>

#include "ChessBoard.h"
// #include "ChessBoardDraw.h"
#include "ChessUtils.h"

ChessData::ChessData() : movesLinkedList(std::make_unique<ChessLinkedListMoves>()) {}

void ChessData::readSimpleGames(const std::string &filename) {
    const auto start = std::chrono::high_resolution_clock::now();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "unable to open the file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        processLine(line);
    }

    file.close();

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "reading the games took: " << duration.count() << " seconds\n";
}

ChessLinkedListMoves *ChessData::getMoves() const { return movesLinkedList.get(); }

ChessData::ResultPair ChessData::getResult(const std::string_view result) {
    std::pair<RESULT, RESULT> gameResult;
    if (result == "1-0") {
        gameResult.first = RESULT::WIN;
        gameResult.second = RESULT::LOSE;
    } else if (result == "0-1") {
        gameResult.first = RESULT::LOSE;
        gameResult.second = RESULT::WIN;
    } else {
        gameResult.first = RESULT::DRAW;
        gameResult.second = RESULT::DRAW;
    }
    return gameResult;
}

void ChessData::processLine(const std::string_view line) {
    std::istringstream iss(line.data());
    std::string pgnMove;

    // get the game result for both colors
    std::string result;
    iss >> result;
    std::pair<RESULT, RESULT> gameResult = getResult(result);

    iss >> pgnMove; // skip second input

    // play the game
    ChessBoard board;
    // ChessBoardDraw boardDraw;
    bool whitesTurn = true;
    while (iss >> pgnMove) {
        addPGNMove(pgnMove, board, whitesTurn, gameResult);
    }

    // set the head back to the root for the next game
    movesLinkedList->setMoveHead(movesLinkedList->getMoveRoot());
}

void ChessData::addPGNMove(const std::string &pgnMove, ChessBoard &board, bool &whitesTurn,
                           const ResultPair &gameResult) {
    const std::string boardMove = ChessUtils::convertPGNToMyInput(pgnMove, board, whitesTurn);
    board.handleMoveInput(boardMove);
    // boardDraw.draw(board); // for debugging
    const std::string boardStr = board.getStringFromBoard();
    movesLinkedList->addMove(boardStr, pgnMove, boardMove, whitesTurn ? gameResult.first : gameResult.second,
                             whitesTurn);
    whitesTurn = !whitesTurn;
}

// example data
/*1-0 : e4 e6 d4 b6 a3 Bb7 Nc3 Nh6 Bxh6 gxh6 Be2 Qg5 Bg4 h5 Nf3 Qg6 Nh4 Qg5 Bxh5
 * Qxh4 Qf3 Kd8 Qxf7 Nc6 Qe8#*/
/*1-0 : d4 d5 Nf3 Nf6 e3 Bf5 Nh4 Bg6 Nxg6 hxg6 Nd2 e6 Bd3 Bd6 e4 dxe4 Nxe4 Rxh2
 * Ke2 Rxh1 Qxh1 Nc6 Bg5 Ke7 Qh7 Nxd4+ Kd2 Qe8 Qxg7 Qh8 Bxf6+ Kd7 Qxh8 Rxh8
 * Bxh8*/
