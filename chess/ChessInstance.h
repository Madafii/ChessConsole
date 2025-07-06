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
    void runWithChessDatabase();
    void runAgainstPeepo();
    void runAgainstDatabase();
    void runWithAnalyzer();

    void loadDB();

  private:
    std::vector<std::string> gameOptions;

    void printGameOptions();
};

#endif // CHESSINSTANCE_H
