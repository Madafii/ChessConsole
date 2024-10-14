//
// Created by finnp on 10/13/24.
//
#include "ChessInstance.h"
#include "gtest/gtest.h"
#include <sstream>
#include <iostream>
#include <string>
void doMovements(std::vector<std::string> moves) {
    ChessBoard board;
    for (std::string move : moves) {
        board.handleMoveInput(move);
        board.updateBoard();    // just enable for visualization
    }
}

TEST(basicChessTests, testChessInput) {
    ChessBoard board;
    board.handleMoveInput("a2:a4");
    board.updateBoard();
}

TEST(basicChessTests, testPawnMovement) {
    const std::vector<std::string> inputs = {"a2:a4", "b7:b5", "a4:b5"};
    doMovements(inputs);
}

TEST(basicChessTests, testBishopMovement) {
    const std::vector<std::string> inputs = {"b2:b3", "a7:a6", "c1:e3"};
    doMovements(inputs);
}

TEST(basicChessTests, testRookMovement) {
    const std::vector<std::string> input = {"a2:a4", "a7:a6", "a1:a3", "h7:h6", "a3:d3"};
    doMovements(input);
}

TEST(basicChessTests, testKnightMovements) {
    const std::vector<std::string> input = {"b1:c3", "g8:f6", "c3:e4"};
    doMovements(input);
}