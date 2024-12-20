#ifndef CHESSDATA_H
#define CHESSDATA_H

#include "ChessLinkedListMoves.h"
#include <string>

class ChessBoard;

class ChessData {
    using ResultPair = std::pair<RESULT, RESULT>;

  public:
    explicit ChessData();

    void readSimpleGames(const std::string &filename);
    // TODO: save linked list and reader for that
    [[nodiscard]] ChessLinkedListMoves *getMoves() const;

  private:
    std::unique_ptr<ChessLinkedListMoves> movesLinkedList;

    ResultPair getResult(std::string_view result);
    void processLine(std::string_view line);
    void addPGNMove(const std::string &pgnMove, ChessBoard &board, bool &whitesTurn, const ResultPair &gameResult);
};

#endif
