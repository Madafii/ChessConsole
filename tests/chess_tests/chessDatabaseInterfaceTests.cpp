#include "ChessDatabaseInterface.h"
#include "ChessData.h"
#include "ChessLinkedListMoves.h"
#include "gtest/gtest.h"
#include <optional>
#include <string>

const std::string dbTestName = "chessMovesTest";
constexpr const char * db = "chessMoves";

TEST(dbInterfaceTests, selectMove) {
    // ChessDatabaseInterface chessDB("chessMoves");
    //
    // int num = 0;
    // bool white = true;
    // DataBits bits("0010000100100001");
    // std::cout << ChessDatabaseInterface::getDBHexStringFromMoveData(bits) << std::endl;
    //
    // auto move1 = chessDB.getMove({num, white}, 1);
    // auto move2 = chessDB.getMove({num, !white}, 1, DataBits("0010000100100001"));
    //
    // EXPECT_TRUE(std::nullopt != move1);
    // EXPECT_TRUE(std::nullopt != move2);
    //
    // std::cout << ChessLinkedListMoves::getBasicInfo(&*move1) << std::endl;
    // std::cout << ChessLinkedListMoves::getBasicInfo(&*move2) << std::endl;
    //
    // auto moves = chessDB.getNextMoves({num, white}, 3);
    // for (const auto &move : moves) {
    //     std::cout << ChessLinkedListMoves::getBasicInfo(&move.second);
    // }
}

// very slow each iteration of reading moves/pushing them takes around 1.5-2.0 seconds totaling at around 3 min
TEST(dbInterfacePerformanceTests, pushMoves5) {
    // ChessData chessData;
    // chessData.pushMovesToDB("/home/finnp/Documents/GitRepos/ChessConsole/tests/Data/liches_db_tests_500.txt", dbTestName, 5);
}

// 11.5-14.0 seconds totaling at 2 min
TEST(dbInterfacePerformanceTests, pushMoves50) {
    // ChessData chessData;
    // chessData.pushMovesToDB("/home/finnp/Documents/GitRepos/ChessConsole/tests/Data/liches_db_tests_100.txt", dbTestName, 50);
}

// test case 500 lines long so total time is 2.4 min
TEST(dbInterfacePerformanceTests, pushMoves500) {
    ChessData chessData;
    chessData.pushMovesToDB("/home/finnp/Documents/GitRepos/ChessConsole/tests/Data/liches_db_tests_500.txt", db, 500);
}

// kinda tested 50000 lines at once which took 30 mins, which is only a litle better than 50 each iteration
