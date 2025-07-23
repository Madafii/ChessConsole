#ifndef CHESSDATA_H
#define CHESSDATA_H

#include "ChessDatabaseInterface.h"
#include "ChessInterface.h"
#include "ChessLinkedListMoves.h"
#include <string>
#include <string_view>

class ChessBoard;

class ChessData {
  public:
    using RecursiveFunc = std::function<void(ChessDatabaseInterface::table_pair &, MoveCompressed &)>;
    using ResultPair = std::pair<RESULT, RESULT>;

    explicit ChessData();

    void readSimpleGames(const std::string &filename);
    void convertPGNToMoves(std::string_view fromFileName, std::string_view toFileName);

    void flushMovesToDB(const std::string &dbName);

    inline void clearMoves() { movesLinkedList.reset(); };

    [[nodiscard]] ChessLinkedListMoves *getMoves() const { return movesLinkedList.get(); };

  private:
    std::unique_ptr<ChessLinkedListMoves> movesLinkedList;

    ResultPair getResult(std::string_view result);
    void processLine(std::string_view line);
    void processLine(std::ofstream &outStream, std::string_view line);
    void addPGNMove(const std::string &pgnMove, ChessInterface &chessInterface, const ResultPair &gameResult);

    static std::ifstream getInputFile(std::string_view inFileName);
    static std::ofstream getOutputFile(std::string_view outFileName);
    static std::string doPGNMove(const std::string &pgnMove, ChessInterface &chessInterface);
};

#endif
