#ifndef CHESSDATA_H
#define CHESSDATA_H

#include <string>
#include "ChessLinkedListMoves.h"

class ChessBoard;

class ChessData {
    using ResultPair = std::pair<RESULT, RESULT>;
public:
    explicit ChessData();

    void readSimpleGames(const std::string &filename);
    ChessLinkedListMoves *getMoves() const;

private:
    std::unique_ptr<ChessLinkedListMoves> movesLinkedList;

    ResultPair getResult(std::string_view result);
    void processLine(std::string_view line);
    void addPGNMove(const std::string &pgnMove, ChessBoard &board, bool &whitesTurn, const ResultPair &gameResult);
};

#endif
