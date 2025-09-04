#include "ChessData.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

// #include "ChessBoard.h"
// #include "ChessBoardDraw.h"

ChessData::ChessData() : movesLinkedList(std::make_unique<ChessLinkedListMoves>()) {}

void ChessData::pushMovesToDB(const std::string_view fromFileName, const std::string_view dbName, const int lineBuffer) {
    std::cout << "started reading the games" << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    std::ifstream fromFile = getInputFile(fromFileName);
    if (!fromFile.is_open()) return;

    while (!fromFile.eof()) {
        readLines(fromFile, lineBuffer);
        flushMovesToDB(dbName);
    }

    fromFile.close();

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "reading the games took: " << duration.count() << " seconds\n";
}

// reads data from pgn file and converts them into a ChessLinkedListMoves structure
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

void ChessData::flushMovesToDB(std::string_view dbName) {
    ChessDatabaseInterface db(dbName.data());

    // simulate its blacks turn because then next move is white, which is the start move
    // db.pushMovesToDBOld(*getMoves(), {0, false});
    try {
        db.pushMovesToDB(*getMoves());
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "SQL state: " << e.sqlstate() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "pushing moves to db error: " << e.what() << std::endl;
    }

    clearMoves();
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

// reads lines returns true when end was reached
void ChessData::readLines(std::ifstream &file, int lines) {
    int lineCounter = 0;
    std::string line;
    while (std::getline(file, line) && lineCounter < lines) {
        processLine(line);
        ++lineCounter;
    }
    std::cout << "processed lines: " << lineCounter << std::endl;
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

// example data
/*1-0 : e4 e6 d4 b6 a3 Bb7 Nc3 Nh6 Bxh6 gxh6 Be2 Qg5 Bg4 h5 Nf3 Qg6 Nh4 Qg5 Bxh5
 * Qxh4 Qf3 Kd8 Qxf7 Nc6 Qe8#*/
/*1-0 : d4 d5 Nf3 Nf6 e3 Bf5 Nh4 Bg6 Nxg6 hxg6 Nd2 e6 Bd3 Bd6 e4 dxe4 Nxe4 Rxh2
 * Ke2 Rxh1 Qxh1 Nc6 Bg5 Ke7 Qh7 Nxd4+ Kd2 Qe8 Qxg7 Qh8 Bxf6+ Kd7 Qxh8 Rxh8
 * Bxh8*/
