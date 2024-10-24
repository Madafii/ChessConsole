//
// Created by finnp on 10/13/24.
//
#include <iostream>
#include <sstream>
#include <string>
#include "ChessInstance.h"
#include "gtest/gtest.h"
using strvec = std::vector<std::string>;
ChessBoard* doMovements(strvec moves) {
    ChessBoard *board = new ChessBoard();
    for (std::string move: moves) {
        board->handleMoveInput(move);
        board->updateBoard(); // just enable for visualization
    }
    return board;
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
    const strvec input = {"f2:f3", "e7:e6", "g2:g4", "d8:h4", "a2:a3"};
    doMovements(input);
}

TEST(basicChessTests, testKingIsChecked) {
    const strvec input = {"f2:f3", "e7:e6", "h2:h3", "d8:h4", "g2:g3"};
    doMovements(input);
}

TEST(basicChessTests, testKingIsPinned) {
    const strvec input = {"f2:f3", "e7:e6", "g2:g3", "d8:h4", "g3:g4"};
    doMovements(input);
}

TEST(basicChessTests, testThatWeirdPawnMovement) {
    const strvec input = {"e2:e4", "a7:a6", "e4:e5", "d7:d5", "e5:d6"};
    doMovements(input);
}

TEST(basicChessTests, testCastling) {
    const strvec input = {"g1:f3", "a7:a6", "g2:g3", "b7:b5", "f1:g2", "c7:c6", "e1:g1"};
    doMovements(input);
}

TEST(basicChessTests, testCastlingLeft) {
    const strvec input = {"d2:d4", "a7:a6", "d1:d3", "b7:b5", "c1:d2", "c7:c6", "b1:c3", "d7:d5", "e1:c1"};
    doMovements(input);
}

TEST(basicChessTests, testCastlingRightNotPossible) {
    const strvec input = {"d2:d4", "a7:a6", "d1:d3", "b7:b5", "c1:d2", "c7:c6", "b1:c3",
                          "d7:d5", "e2:e3", "e7:e6", "b2:b3", "f8:a3", "e1:c1"};
    doMovements(input);
}

TEST(basicChessTests, testPawnWon) {
    const strvec input = {"a2:a4", "h7:h6", "a4:a5", "h6:h5", "a5:a6", "h5:h4", "a6:b7", "h4:h3", "b7:a8" };
    const std::string newPawnType = "Q";
    const std::istringstream iss(newPawnType);
    std::cin.rdbuf(iss.rdbuf());

    doMovements(input);
}
