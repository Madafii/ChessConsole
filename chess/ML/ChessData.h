#ifndef CHESSDATA_H
#define CHESSDATA_H

#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include <string>

class ChessBoard;

class ChessData {
  public:
    using RecursiveFunc = std::function<void(ChessDatabaseInterface::table_pair &, MoveCompressed &)>;
    using ResultPair = std::pair<RESULT, RESULT>;

    explicit ChessData();

    void readSimpleGames(const std::string &filename);
    void flushMovesToDB(const std::string &dbName);

    void clearMoves();

    // TODO: save linked list and reader for that
    [[nodiscard]] ChessLinkedListMoves *getMoves() const;

  private:
    std::unique_ptr<ChessLinkedListMoves> movesLinkedList;

    ResultPair getResult(std::string_view result);
    void processLine(std::string_view line);
    void addPGNMove(const std::string &pgnMove, ChessBoard &board, bool &whitesTurn, const ResultPair &gameResult);
};

#endif
