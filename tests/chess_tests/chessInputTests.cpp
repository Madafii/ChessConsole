//
// Created by finnp on 10/13/24.
//
#include <iostream>
#include <sstream>
#include <string>
#include "ChessInstance.h"
#include "gtest/gtest.h"
using strvec = std::vector<std::string>;
void doMovements(strvec moves) {
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
    const strvec inputs = {"a2:a4", "b7:b5", "a4:b5"};
    doMovements(inputs);
}

TEST(basicChessTests, testBishopMovement) {
    const strvec inputs = {"b2:b3", "a7:a6", "c1:a3"};
    doMovements(inputs);
}

TEST(basicChessTests, testRookMovement) {
    const strvec input = {"a2:a4", "a7:a6", "a1:a3", "h7:h6", "a3:d3"};
    doMovements(input);
}

TEST(basicChessTests, testKnightMovements) {
    const strvec input = {"b1:c3", "g8:f6", "c3:e4"};
    doMovements(input);
}

TEST(basicChessTests, testKingIsCheckmate) {
    const strvec input = {"f2:f3", "e7:e6", "g2:g4", "d8:h4", "a2:a3" };
    doMovements(input);
}

TEST(basicChessTests, testKingIsChecked) {
    const strvec input = {"f2:f3", "e7:e6", "h2:h3", "d8:h4", "g2:g3" };
    doMovements(input);
}

TEST(basicChessTests, testKingIsPinned) {
    const strvec input = {"f2:f3", "e7:e6", "g2:g3", "d8:h4", "g3:g4" };
    doMovements(input);
}
