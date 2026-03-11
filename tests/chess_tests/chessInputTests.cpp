#include "ChessBoard.h"
#include "ChessInterface.h"
#include "ChessUtils.h"
#include "config.hpp"

#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using StrVec = std::vector<std::string>;
using StrVecPair = std::vector<std::pair<std::string, std::string>>;
using MoveBoardData = std::vector<std::pair<std::string, std::vector<std::string>>>;
std::filesystem::path dataDir = TEST_DATA_DIR;

void doMovements(const StrVec &moves) {
    ChessInterface chessInterface;
    for (const std::string &move : moves) {
        if (chessInterface.handleMoveInput(move).value() != GameState::InProgress) break;
    }
}

void doMovements(ChessInterface &chessInterface, const StrVec &moves) {
    for (const std::string &move : moves) {
        auto gameState = chessInterface.handleMoveInput(move);
        if (!gameState) {
            std::cout << "invalid input" << std::endl;
            return;
        }
        if (*gameState != GameState::InProgress) break;
    }
}

void doMovements(ChessBoard &chessBoard, const StrVecPair &moves) {
    for (const auto &[from, to] : moves) {
        auto &fromTile = chessBoard.getTileAt(from);
        auto &toTile = chessBoard.getTileAt(to);
        chessBoard.makeMove(fromTile, toTile);
    }
}

void testUndo(const StrVecPair &moves) {
    ChessBoard board;
    for (auto it = moves.begin(); it != moves.end() - 1; it++) {
        auto &fromTile = board.getTileAt(it->first);
        auto &toTile = board.getTileAt(it->second);
        board.makeMove(fromTile, toTile);
    }
    const std::string previousBoardStr = board.getStringFromBoard();

    const auto &[lastFrom, lastTo] = moves.back();
    auto &fromTile = board.getTileAt(lastFrom);
    auto &toTile = board.getTileAt(lastTo);
    board.makeMove(fromTile, toTile);
    EXPECT_NE(previousBoardStr, board.getStringFromBoard());

    // undo capturing piece
    board.undoMove();
    EXPECT_EQ(previousBoardStr, board.getStringFromBoard());
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

GameState doMovementsFromPGN(ChessInterface &chessInterface, const StrVec &moves) {
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

// ------------------------------TESTING AREA------------------------------------------------- //

TEST(ChessInterfaceTests, testBasics) {
    auto movesTests = readInputFile(dataDir.string() + "/basicMoves.txt");

    for (const auto &[checkBoard, moves] : movesTests) {
        ChessInterface chessInterface;
        doMovements(chessInterface, moves);
        EXPECT_EQ(checkBoard, chessInterface.getChessBoard().getGameHistory().back());
    }
}

TEST(ChessInterfaceTests, testCheckmates) {
    auto movesTests = readInputFile(dataDir.string() + "/checkmates.txt");

    for (const auto &[gameOutcome, moves] : movesTests) {
        ChessInterface chessInterface;
        GameState gameState = GameState::InProgress;
        bool winningColor = gameOutcome[0] == '1';
        for (const auto &move : moves) {
            gameState = chessInterface.handleMoveInput(move).value();
        }

        // someone won and it is the correct color
        EXPECT_EQ(gameState, GameState::Won);
        EXPECT_EQ(winningColor, !chessInterface.getChessBoard().isWhitesTurn());
    }
}

TEST(basicChessTests, testPGNDraw) {
    const StrVec input = {"e4",   "e5",   "Nf3",  "d6",    "Bc4",  "a5",   "Nc3",  "c6",   "O-O",  "b5",   "Be2",  "a4",   "a3",   "Be7",
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

    EXPECT_EQ(GameState::Draw, endState);
}

TEST(basicChessTests, testPGNConverterWin) {
    const StrVec input = {"e4",  "d5",  "exd5", "Qxd5", "Nc3",   "Qa5",  "Nf3",  "Nf6",  "d4",    "c6",  "Bg5", "Ng8",
                          "Bd2", "Nf6", "Ne4",  "Qb6",  "Nxf6+", "exf6", "Qe2+", "Be6",  "O-O-O", "Be7", "g3",  "Bxa2",
                          "b3",  "c5",  "Kb2",  "cxd4", "Kxa2",  "Nc6",  "Bf4",  "Qa5+", "Kb2",   "Nb4", "Ra1", "Qf5",
                          "Bd6", "Nd5", "Nxd4", "Qg6",  "Bg2",   "Qg5",  "Bxd5", "Qxd5", "Qxe7#"};
    ChessInterface chessInterface;
    doMovementsFromPGN(chessInterface, input);
}

TEST(basicChessTests, testPGN50Moves) {
    const StrVec input = {
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
    const StrVec input = {"a2:a4", "h7:h5", "a1:a2", "h8:h7", "a2:a1", "h7:h8", "a1:a2",
                          "h8:h7", "a2:a1", "h7:h8", "a1:a2", "h8:h7", "a2:a1", "h7:h8"};
    doMovements(input);
}

TEST(basicChessTests, testTmp) {
    const StrVec input = {"d4",  "d5",  "Bd2",  "Nc6",  "Nf3",  "Nf6", "c4",   "dxc4",  "Nc3", "Nxd4", "Nxd4", "Qxd4", "e3",   "Qc5",
                          "Qa4", "Bd7", "Qxc4", "Qxc4", "Bxc4", "e6",  "O-O",  "O-O-O", "Be1", "a6",   "e4",   "Bc6",  "f3",   "Bc5",
                          "Bf2", "Nd7", "Rad1", "Bxf2", "Kxf2", "Nc5", "Rxd8", "Rxd8",  "Ke3", "Bb5",  "Bxb5", "axb5", "Nxb5", "Rd3",
                          "Ke2", "Rd7", "Rc1",  "b6",   "b4",   "Na6", "Na7",  "Kb7",   "Nb5", "Nxb4", "a4",   "c6",   "Nc3",  "Ka6",
                          "Rb1", "Ka5", "h4",   "Nd3",  "g4",   "Nf4", "Ke3",  "e5",    "h5",  "Rd3",  "Kf2",  "Rxc3", "Rd1",  "Rc2",
                          "Kg3", "Rg2", "Kh4",  "f6",   "Rd6",  "h6",  "Rd7",  "g5",    "hg6"};
    ChessInterface chessInterface;
    doMovementsFromPGN(chessInterface, input);
}

TEST(testUndo, testInitialMove) {
    const StrVecPair input = {{"a1", "a3"}};
    testUndo(input);
}

TEST(testUndo, testCapturing) {
    const StrVecPair input = {{"a1", "a3"}, {"b6", "b4"}, {"a1", "a3"}};
    testUndo(input);
}

TEST(testUndo, testEnPassant) {
    const StrVecPair inputs = {{"a1", "a3"}, {"a6", "a5"}, {"a3", "a4"}, {"b6", "b4"}};
    testUndo(inputs);
}

TEST(testUndo, testCastling) {
    const StrVecPair inputs = {{"g0", "h2"}, {"a6", "a5"}, {"e1", "e2"}, {"a5", "a4"}, {"f0", "e1"}, {"a4", "a3"}, {"e0", "g0"}};
    testUndo(inputs);
}

TEST(testUndo, testRepeatedUndo) {
    const StrVecPair input1 = {{"a1", "a3"}};
    const StrVecPair input2 = {{"b6", "b4"}};
    ChessBoard board;
    doMovements(board, input1);
    const std::string previousBoardStr = board.getStringFromBoard();
    doMovements(board, input2);

    EXPECT_NE(previousBoardStr, board.getStringFromBoard());
    board.undoMove();
    EXPECT_EQ(previousBoardStr, board.getStringFromBoard());
    // second undo should not do anything
    board.undoMove();
    EXPECT_EQ(previousBoardStr, board.getStringFromBoard());
}
