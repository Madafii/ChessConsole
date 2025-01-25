//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSINSTANCE_H
#define CHESSINSTANCE_H

#include <string>
#include <vector>

class ChessInstance {
  public:
    explicit ChessInstance();
    ~ChessInstance();

    void run();
    void runRandom();
    void runAgainstRandom(bool white);
    void runWithChessData();
    void runAgainstPeepo();
    void runWithAnalyzer();

  private:
    std::vector<std::string> gameOptions;

    void printGameOptions();
};

#endif // CHESSINSTANCE_H
