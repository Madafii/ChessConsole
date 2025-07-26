#ifndef CHESSPEEPO_H
#define CHESSPEEPO_H

#include "ChessBoard.h"
#include "ChessData.h"
#include "ChessInterface.h"
#include "ChessLinkedListMoves.h"
#include "ChessDatabaseInterface.h"
#include <sys/types.h>

class ChessPeepo {
  public:
    explicit ChessPeepo(ChessInterface &setChessInterface, ChessData &data);

    // all the moves peeop can do
    GameState makeMostPlayedMove();
    void makeHighesWinRateMove();
    void makeBestMove();
    void makeRandomBestMove(const int &randomRange = -1);
    void makeRandomMove();

    bool makeBestDBMove();

    static MoveCompressed *getMostPlayedMove(const std::vector<MoveCompressed*> &moves);
    static std::string getRandomInputMove(ChessInterface &chessI);

    inline static uint64_t playedMoves(const MoveCompressed *move) { return move->wins + move->loses + move->draws; }

  private:
    ChessInterface &chessInterface;
    ChessData &data;

    /*std::vector<std::string> getSmartMoveSelection();*/
    /*std::vector<std::string> getMoveSelection();*/


};

#endif // CHESSPEEPO_H
