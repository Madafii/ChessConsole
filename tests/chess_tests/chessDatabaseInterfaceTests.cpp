#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include "gtest/gtest.h"
#include <optional>
#include <string>

TEST(dbInterfaceTests, selectMove) {
    ChessDatabaseInterface chessDB("chessMoves");

    int num = 0;
    bool white = true;
    DataBits bits("0010000100100001");
    std::cout << ChessDatabaseInterface::getDBHexStringFromMoveData(bits) << std::endl;

    auto move1 = chessDB.getMove({num, white}, 1);
    auto move2 = chessDB.getMove({num, !white}, 1, DataBits("0010000100100001"));

    EXPECT_TRUE(std::nullopt != move1);
    EXPECT_TRUE(std::nullopt != move2);

    std::cout << ChessLinkedListMoves::getBasicInfo(&*move1) << std::endl;
    std::cout << ChessLinkedListMoves::getBasicInfo(&*move2) << std::endl;

    auto moves = chessDB.getNextMoves({num, white}, 3);
    for (const auto &move : moves) {
        std::cout << ChessLinkedListMoves::getBasicInfo(&move.second);
    }
}
