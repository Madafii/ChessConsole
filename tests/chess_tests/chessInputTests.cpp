//
// Created by finnp on 10/13/24.
//
#include "ChessUtils.h"
#include "gtest/gtest.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using strvec = std::vector<std::string>;

void doMovements(strvec moves) {
    ChessBoard board;
    for (const std::string move : moves) {
        if (board.handleMoveInput(move) != GameState::IN_PROGRESS)
            break;
    }
}

void doMovementsFromPGN(strvec moves) {
    ChessBoard board;
    bool whitesTurn = true;
    int count = 0;
    for (const std::string move : moves) {
        std::string inputMyChess = ChessUtils::convertPGNToMyInput(move, board, whitesTurn);
        std::cout << "from: " << move << " to: " << inputMyChess << std::endl;
        if (board.handleMoveInput(inputMyChess) != GameState::IN_PROGRESS)
            break;
        whitesTurn = !whitesTurn;
        count++;
    }
}

TEST(basicChessTests, testChessInput) {
    ChessBoard board;
    board.handleMoveInput("a2:a4");
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

TEST(basicChessTests, testCastlingLeftNotPossible) {
    const strvec input = {"d2:d4", "a7:a6", "d1:d3", "b7:b5", "c1:d2", "c7:c6", "b1:c3",
                          "d7:d5", "e2:e3", "e7:e6", "b2:b3", "f8:a3", "e1:c1"};
    doMovements(input);
}

TEST(basicChessTests, testPawnWon) {
    const strvec input = {"a2:a4", "h7:h6", "a4:a5", "h6:h5", "a5:a6", "h5:h4", "a6:b7", "h4:h3", "b7:a8"};
    const std::string newPawnType = "Q";
    const std::istringstream iss(newPawnType);
    std::cin.rdbuf(iss.rdbuf());

    doMovements(input);
}

TEST(basicChessTests, testPawnWon2) {
    const strvec input = {"a2:a4", "h7:h6", "a4:a5", "h6:h5", "a5:a6", "h5:h4", "a6:b7", "h4:h3", "b7:a8=N"};
    doMovements(input);
}

TEST(basicChessTests, testPGNConverterDraw) {
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
    doMovementsFromPGN(input);
}

TEST(basicChessTests, testPGNConverterWin) {
    const strvec input = {"e4",  "d5",  "exd5", "Qxd5", "Nc3",   "Qa5",  "Nf3",  "Nf6",  "d4",    "c6",  "Bg5", "Ng8",
                          "Bd2", "Nf6", "Ne4",  "Qb6",  "Nxf6+", "exf6", "Qe2+", "Be6",  "O-O-O", "Be7", "g3",  "Bxa2",
                          "b3",  "c5",  "Kb2",  "cxd4", "Kxa2",  "Nc6",  "Bf4",  "Qa5+", "Kb2",   "Nb4", "Ra1", "Qf5",
                          "Bd6", "Nd5", "Nxd4", "Qg6",  "Bg2",   "Qg5",  "Bxd5", "Qxd5", "Qxe7#"};
    doMovementsFromPGN(input);
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
    doMovementsFromPGN(input);
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
    doMovementsFromPGN(input);
}
