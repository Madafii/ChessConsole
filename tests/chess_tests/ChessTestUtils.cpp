#include "ChessBoard.h"
#include "ChessInterface.h"
#include "ChessUtils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using strvec = std::vector<std::string>;
using MoveBoardData = std::vector<std::pair<std::string, std::vector<std::string>>>;
static constexpr char * testDataDir = "./tests/Data";

void doMovements(const strvec &moves) {
    ChessInterface chessInterface;
    for (const std::string &move : moves) {
        if (chessInterface.handleMoveInput(move).value() != GameState::InProgress) break;
    }
}

void doMovements(ChessInterface &chessInterface, const strvec &moves) {
    for (const std::string &move : moves) {
        auto gameState = chessInterface.handleMoveInput(move);
        if (!gameState) {
            std::cout << "invalid input" << std::endl;
            return;
        }
        if (*gameState != GameState::InProgress) break;
    }
}

auto readInputFile(const std::string &fileName) -> MoveBoardData {
    std::ifstream inputDataFile(fileName);
    std::string line;

    if (!inputDataFile.is_open()) {
        std::cerr << "could not open file" << std::endl;
        return {};
    }

    MoveBoardData moveTests;

    while (std::getline(inputDataFile, line)) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (iss >> token) {
            tokens.push_back(token);
        }
        std::vector moves(tokens.begin(), tokens.end() - 1);
        moveTests.emplace_back(tokens.back(), moves);
    }
    return moveTests;
}

GameState doMovementsFromPGN(ChessInterface &chessInterface, const strvec &moves) {
    GameState currState = GameState::InProgress;
    for (const std::string &move : moves) {
        const bool white = chessInterface.getChessBoard().isWhitesTurn();
        std::string inputMyChess = ChessUtils::convertPGNToMyInput(move, chessInterface.getChessMoveLogic(), white);
        auto currState = chessInterface.handleMoveInput(inputMyChess);
        if (!currState) {
            std::cerr << "reading PGN input should never result in a wrong input" << std::endl;
        }
        if (*currState != GameState::InProgress) break;
    }
    return currState;
}
