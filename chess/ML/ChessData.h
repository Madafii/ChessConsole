#ifndef CHESSDATA_H
#define CHESSDATA_H

#include "ChessDatabaseInterface.h"
#include "ChessInterface.h"
#include "ChessLinkedListMoves.h"
#include "ChessUtils.h"
#include <fstream>
#include <string>
#include <string_view>

class ChessData {
  public:
    using RecursiveFunc = std::function<void(ChessDatabaseInterface::table_pair &, MoveCompressed &)>;
    using ResultPair = std::pair<RESULT, RESULT>;

    explicit ChessData();

    void pushMovesToDB(std::string_view fromFileName, std::string_view dbName);
    void readSimpleGames(const std::string &filename);
    void flushMovesToDB(std::string_view dbName);

    static void convertPGNToMoves(std::string_view fromFileName, std::string_view toFileName);

    inline void clearMoves() { movesLinkedList.reset(); };

    [[nodiscard]] ChessLinkedListMoves *getMoves() const { return movesLinkedList.get(); };

  private:
    std::unique_ptr<ChessLinkedListMoves> movesLinkedList;

    ResultPair getResult(std::string_view result);
    void processLine(std::string_view line);

    void addPGNMove(const std::string &pgnMove, ChessInterface &chessInterface, const ResultPair &gameResult) {
        const std::string boardMove = doPGNMove(pgnMove, chessInterface);
        bool white = chessInterface.getChessBoard().isWhitesTurn();
        movesLinkedList->addMoveCompressed(boardMove, white ? gameResult.first : gameResult.second, white);
    }

    void readLines(std::ifstream &file, int lines);

    // static conversion
    static void processLine(std::ofstream &outStream, std::string_view line);
    static std::string doPGNMove(const std::string &pgnMove, ChessInterface &chessInterface) {
        bool white = chessInterface.getChessBoard().isWhitesTurn();
        const std::string boardMove = ChessUtils::convertPGNToMyInput(pgnMove, chessInterface.getChessMoveLogic(), white);
        chessInterface.handleMoveInputNoChecks(boardMove, false);
        return boardMove;
    }

    static std::ifstream getInputFile(std::string_view inFileName);
    static std::ofstream getOutputFile(std::string_view outFileName);
};

#endif
