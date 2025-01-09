#ifndef CHESSANALYZER_H
#define CHESSANALYZER_H

#include "ChessBoard.h"

class ChessAnalyzer {
  public:
    explicit ChessAnalyzer(const ChessBoard &aboard);

    std::optional<std::vector<std::string>> getForcedCheckmate(int depth);

  private:
    const ChessBoard &origBoard;
};

#endif
