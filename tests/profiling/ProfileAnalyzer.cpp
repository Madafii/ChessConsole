#include "ChessConsoleUI.h"
#include "ChessPlayerAnalyzer.h"
#include <gtest/gtest.h>

TEST(ProfileAnalyzer, AnaAgainstAna) {
    ChessBoardDrawSettings settings(false, true);
    ChessConsoleUI cc([](ChessInterface &chessInterface) { return std::make_unique<ChessPlayerAnalyzer>(chessInterface); },
                      [](ChessInterface &chessInterface) { return std::make_unique<ChessPlayerAnalyzer>(chessInterface); }, settings);
    cc.startSteps(5);
}
