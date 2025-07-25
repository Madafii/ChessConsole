#include "ChessData.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "ChessBoard.h"
// #include "ChessBoardDraw.h"
#include "ChessDatabaseInterface.h"
#include "ChessInterface.h"
#include "ChessLinkedListMoves.h"
#include "ChessUtils.h"

ChessData::ChessData() : movesLinkedList(std::make_unique<ChessLinkedListMoves>()) {}

void ChessData::readSimpleGames(const std::string &filename) {
    const auto start = std::chrono::high_resolution_clock::now();

    std::ifstream file = getInputFile(filename);
    if (!file) return;

    int lineCounter = 0;
    std::string line;
    while (std::getline(file, line)) {
        processLine(line);
        ++lineCounter;
        std::cout << "processed lines: " << lineCounter << std::endl;
    }

    file.close();

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "reading the games took: " << duration.count() << " seconds\n";
}

void ChessData::convertPGNToMoves(std::string_view fromFileName, std::string_view toFileName) {
    const auto start = std::chrono::high_resolution_clock::now();

    std::ifstream inFile = getInputFile(fromFileName);
    if (!inFile) return;

    std::ofstream outFile = getOutputFile(toFileName);
    if (!outFile) return;

    int lineCounter = 0;
    std::string line;
    while (std::getline(inFile, line)) {
        processLine(outFile, line);
        ++lineCounter;
        std::cout << "processed lines: " << lineCounter << std::endl;
    }

    inFile.close();

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "reading the games took: " << duration.count() << " seconds\n";
}

void ChessData::flushMovesToDB(const std::string &dbName) {
    ChessDatabaseInterface db(dbName);

    // simulate its blacks turn because then next move is white, which is the start move
    db.pushMovesToDB(*getMoves(), {0, false});

    clearMoves();
}

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
    ChessInterface chessInterface;
    while (iss >> pgnMove) {
        addPGNMove(pgnMove, chessInterface, gameResult);
    }

    // set the head back to the root for the next game
    movesLinkedList->setMoveHead(movesLinkedList->getMoveRoot());
}

void ChessData::processLine(std::ofstream &outStream, std::string_view line) {
    std::istringstream iss(line.data());
    std::string pgnMove;

    // skip first two entries
    iss >> pgnMove >> pgnMove;

    // play the game
    ChessInterface chessInterface;
    while (iss >> pgnMove) {
        const std::string boardMove = doPGNMove(pgnMove, chessInterface);
        outStream << boardMove << " ";
    }
    outStream << std::endl;
}

void ChessData::addPGNMove(const std::string &pgnMove, ChessInterface &chessInterface, const ResultPair &gameResult) {
    const std::string boardMove = doPGNMove(pgnMove, chessInterface);
    bool white = chessInterface.getChessBoard().isWhitesTurn();
    movesLinkedList->addMoveCompressed(boardMove, white ? gameResult.first : gameResult.second, white);
}

std::ifstream ChessData::getInputFile(std::string_view inFileName) {
    std::ifstream inFile(inFileName.data());
    if (!inFile.is_open()) {
        std::cerr << "unable to open the file: " << inFileName << std::endl;
        return std::ifstream{};
    }
    return inFile;
}

std::ofstream ChessData::getOutputFile(std::string_view outFileName) {
    std::ofstream outFile(outFileName.data());
    if (!outFile.is_open()) {
        std::cerr << "unable to open the file: " << outFileName << std::endl;
        return std::ofstream{};
    }
    return outFile;
}

std::string ChessData::doPGNMove(const std::string &pgnMove, ChessInterface &chessInterface) {
    bool white = chessInterface.getChessBoard().isWhitesTurn();
    const std::string boardMove = ChessUtils::convertPGNToMyInput(pgnMove, chessInterface.getChessMoveLogic(), white);
    chessInterface.handleMoveInputNoChecks(boardMove, false);
    return boardMove;
}
// example data
/*1-0 : e4 e6 d4 b6 a3 Bb7 Nc3 Nh6 Bxh6 gxh6 Be2 Qg5 Bg4 h5 Nf3 Qg6 Nh4 Qg5 Bxh5
 * Qxh4 Qf3 Kd8 Qxf7 Nc6 Qe8#*/
/*1-0 : d4 d5 Nf3 Nf6 e3 Bf5 Nh4 Bg6 Nxg6 hxg6 Nd2 e6 Bd3 Bd6 e4 dxe4 Nxe4 Rxh2
 * Ke2 Rxh1 Qxh1 Nc6 Bg5 Ke7 Qh7 Nxd4+ Kd2 Qe8 Qxg7 Qh8 Bxf6+ Kd7 Qxh8 Rxh8
 * Bxh8*/
