#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessInterface.h"
#include "ChessUtils.h"

#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using strvec = std::vector<std::string>;
using namespace std;

void doMovements(const strvec &moves) {
    ChessInterface chessInterface;
    for (const std::string &move : moves) {
        if (chessInterface.handleMoveInput(move) != GameState::IN_PROGRESS) break;
    }
}

void doMovements(ChessInterface &chessInterface, const strvec &moves) {
    for (const std::string &move : moves) {
        if (chessInterface.handleMoveInput(move) != GameState::IN_PROGRESS) break;
    }
}

auto readInputFile(const string &fileName) -> vector<pair<string, vector<string>>> {
    std::ifstream inputDataFile(fileName);
    std::string line;

    if (!inputDataFile.is_open()) {
        std::cerr << "could not open file" << std::endl;
        return {};
    }

    vector<pair<string, vector<string>>> moveTests;

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
    GameState currState = GameState::IN_PROGRESS;
    for (const std::string &move : moves) {
        const bool white = chessInterface.getChessBoard().isWhitesTurn();
        std::string inputMyChess = ChessUtils::convertPGNToMyInput(move, chessInterface.getChessMoveLogic(), white);
        currState = chessInterface.handleMoveInput(inputMyChess);
        if (currState != GameState::IN_PROGRESS) break;
    }
    return currState;
}

TEST(ChessInterfaceTests, testBasics) {
    auto movesTests = readInputFile("/home/finnp/Documents/GitRepos/ChessConsole/tests/Data/basicMoves.txt");

    for (const auto &[checkBoard, moves] : movesTests) {
        ChessInterface chessInterface;
        doMovements(chessInterface, moves);
        EXPECT_EQ(checkBoard, chessInterface.getChessBoard().getGameHistory().back());
    }
}

TEST(ChessInterfaceTests, testCheckmates) {
    auto movesTests = readInputFile("/home/finnp/Documents/GitRepos/ChessConsole/tests/Data/checkmates.txt");

    for (const auto &[gameOutcome, moves] : movesTests) {
        ChessInterface chessInterface;
        GameState gameState = GameState::IN_PROGRESS;
        bool winningColor = gameOutcome[0] == '1';
        for (const auto &move : moves) {
            gameState = chessInterface.handleMoveInput(move);
        }

        // someone won and it is the correct color
        EXPECT_EQ(gameState, GameState::WON);
        EXPECT_EQ(winningColor, !chessInterface.getChessBoard().isWhitesTurn());
    }
}

TEST(basicChessTests, testPGNDraw) {
    const strvec input = {"e4",   "e5",   "Nf3",  "d6",    "Bc4",  "a5",   "Nc3",  "c6",   "O-O",  "b5",   "Be2",  "a4",   "a3",   "Be7",
                          "d4",   "exd4", "Nxd4", "Nf6",   "Nf5",  "Bxf5", "exf5", "d5",   "Bf3",  "Qc8",  "Qe2",  "Qxf5", "Re1",  "O-O",
                          "Qxe7", "Qd7",  "Qxd7", "Nbxd7", "Bg5",  "Ne8",  "Rad1", "Nb6",  "Be7",  "Nc4",  "Bxf8", "Kxf8", "b3",   "axb3",
                          "cxb3", "Nb2",  "Ra1",  "Nd3",   "Re2",  "Nc5",  "b4",   "Nb3",  "Raa2", "g6",   "Bg4",  "d4",   "Ne4",  "Rd8",
                          "Nd2",  "Nc1",  "Ne4",  "d3",    "Red2", "Nxa2", "Rxa2", "f5",   "Bxf5", "gxf5", "Nd2",  "f4",   "a4",   "bxa4",
                          "Rxa4", "Nd6",  "g3",   "fxg3",  "hxg3", "Rc8",  "Ra6",  "Nb5",  "Ne4",  "Nd4",  "Nd6",  "Rc7",  "Ra8+", "Ke7",
                          "Ne4",  "Ke6",  "Nc5+", "Kd5",   "Nxd3", "c5",   "bxc5", "Rxc5", "Nxc5", "Kxc5", "Rc8+", "Kb6",  "Rh8",  "Ne2+",
                          "Kg2",  "Kc5",  "Rxh7", "Kd4",   "Rh8",  "Nc3",  "Rh4+", "Kd3",  "g4",   "Ne4",  "f3",   "Ng5",  "f4",   "Ne4",
                          "g5",   "Ke3",  "Rg4",  "Nf2",   "g6",   "Nxg4", "g7",   "Kxf4", "g8=Q", "Ne3+", "Kf2",  "Ng4+", "Ke1",  "Ke3",
                          "Qxg4", "Kd3",  "Qg3+", "Ke4",   "Ke2",  "Kd5",  "Qg4",  "Ke5",  "Kd3",  "Kf6",  "Qf4+", "Ke6",  "Kd4",  "Ke7",
                          "Qe5+", "Kf7",  "Kd5",  "Kg6",   "Kd6",  "Kh6",  "Ke6",  "Kg6",  "Qf6+", "Kh5",  "Kf5"};

    ChessInterface chessInterface;
    GameState endState = doMovementsFromPGN(chessInterface, input);

    EXPECT_EQ(GameState::DRAW, endState);
}

TEST(basicChessTests, testPGNConverterWin) {
    const strvec input = {"e4",  "d5",  "exd5", "Qxd5", "Nc3",   "Qa5",  "Nf3",  "Nf6",  "d4",    "c6",  "Bg5", "Ng8",
                          "Bd2", "Nf6", "Ne4",  "Qb6",  "Nxf6+", "exf6", "Qe2+", "Be6",  "O-O-O", "Be7", "g3",  "Bxa2",
                          "b3",  "c5",  "Kb2",  "cxd4", "Kxa2",  "Nc6",  "Bf4",  "Qa5+", "Kb2",   "Nb4", "Ra1", "Qf5",
                          "Bd6", "Nd5", "Nxd4", "Qg6",  "Bg2",   "Qg5",  "Bxd5", "Qxd5", "Qxe7#"};
    ChessInterface chessInterface;
    doMovementsFromPGN(chessInterface, input);
}

TEST(basicChessTests, testPGN50Moves) {
    const strvec input = {
        "a4",  "h5",  "Ra3", "Rh6", "Rh3", "Ra6", "Rg3", "Rb6", "Rf3", "Rc6", "Re3", "Rd6", "Rd3", "Re6", "Rc3", "Rf6", "Rb3", "Rg6", "Ra3",
        "Rh6", "Rb3", "Rg6", "Rc3", "Rf6", "Rd3", "Re6", "Re3", "Rd6", "Rf3", "Rc6", "Rg3", "Rb6", "Rh3", "Ra6", "Rg3", "Rb6", "Rf3", "Rc6",
        "Re3", "Rd6", "Rd3", "Re6", "Rc3", "Rf6", "Rb3", "Rg6", "Ra3", "Rh6", "Rb3", "Rg6", "Rc3", "Rf6", "Rd3", "Re6", "Re3", "Rd6", "Rf3",
        "Rc6", "Rg3", "Rb6", "Rh3", "Ra6", "Rg3", "Rb6", "Rf3", "Rc6", "Re3", "Rd6", "Rd3", "Re6", "Rc3", "Rf6", "Rb3", "Rg6", "Ra3", "Rh6",
        "Rb3", "Rg6", "Rc3", "Rf6", "Rd3", "Re6", "Re3", "Rd6", "Rf3", "Rc6", "Rg3", "Rb6", "Rh3", "Ra6", "Rg3", "Rb6", "Rf3", "Rc6", "Re3",
        "Rd6", "Rd3", "Re6", "Rc3", "Rf6", "Rb3", "Rg6", "Ra3", "Rh6", "Rb3", "Rg6", "Rc3", "Rf6", "Rd3", "Re6", "Re3", "Rd6", "Rf3", "Rc6",
        "Rg3", "Rb6", "Rh3", "Ra6", "Rg3", "Rb6", "Rf3", "Rc6", "Re3", "Rd6", "Rd3", "Re6", "Rc3", "Rf6", "Rb3", "Rg6", "Ra3", "Rh6", "Rb3",
        "Rg6", "Rc3", "Rf6", "Rd3", "Re6", "Re3", "Rd6", "Rf3", "Rc6", "Rg3", "Rb6", "Rh3", "Ra6", "Rg3", "Rb6", "Rf3", "Rc6"};
    ChessInterface chessInterface;
    doMovementsFromPGN(chessInterface, input);
}

TEST(basicChessTests, testThreefoldRepetition) {
    const strvec input = {"a2:a4", "h7:h5", "a1:a2", "h8:h7", "a2:a1", "h7:h8", "a1:a2",
                          "h8:h7", "a2:a1", "h7:h8", "a1:a2", "h8:h7", "a2:a1", "h7:h8"};
    doMovements(input);
}

TEST(basicChessTests, testTmp) {
    const strvec input = {"d4",  "d5",  "Bd2",  "Nc6",  "Nf3",  "Nf6", "c4",   "dxc4",  "Nc3", "Nxd4", "Nxd4", "Qxd4", "e3",   "Qc5",
                          "Qa4", "Bd7", "Qxc4", "Qxc4", "Bxc4", "e6",  "O-O",  "O-O-O", "Be1", "a6",   "e4",   "Bc6",  "f3",   "Bc5",
                          "Bf2", "Nd7", "Rad1", "Bxf2", "Kxf2", "Nc5", "Rxd8", "Rxd8",  "Ke3", "Bb5",  "Bxb5", "axb5", "Nxb5", "Rd3",
                          "Ke2", "Rd7", "Rc1",  "b6",   "b4",   "Na6", "Na7",  "Kb7",   "Nb5", "Nxb4", "a4",   "c6",   "Nc3",  "Ka6",
                          "Rb1", "Ka5", "h4",   "Nd3",  "g4",   "Nf4", "Ke3",  "e5",    "h5",  "Rd3",  "Kf2",  "Rxc3", "Rd1",  "Rc2",
                          "Kg3", "Rg2", "Kh4",  "f6",   "Rd6",  "h6",  "Rd7",  "g5",    "hg6"};
    ChessInterface chessInterface;
    doMovementsFromPGN(chessInterface, input);
}
