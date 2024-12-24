#ifndef CHESSPEEPO_H
#define CHESSPEEPO_H

#include "ChessBoard.h"
#include "ChessData.h"
#include "ChessLinkedListMoves.h"
#include <ranges>
#include <sys/types.h>

class ChessPeepo {
  public:
    explicit ChessPeepo(ChessBoard &board, ChessData &data);

    // all the moves peeop can do
    GameState makeMostPlayedMove();
    void makeHighesWinRateMove();
    void makeBestMove();
    void makeRandomBestMove(const int &randomRange = -1);
    void makeRandomMove();

    // some random checks
    bool isBoardDataOnSync();

  private:
    ChessBoard &board;
    ChessData &data;

    /*std::vector<std::string> getSmartMoveSelection();*/
    /*std::vector<std::string> getMoveSelection();*/

    MoveCompressed *getMostPlayedMove(const std::vector<std::unique_ptr<MoveCompressed>> &moves);
    std::string getRandomInputMove();

    u_int64_t playedMoves(const MoveCompressed *move);
};

#endif // CHESSPEEPO_H
