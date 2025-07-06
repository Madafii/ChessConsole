#ifndef CHESSINSTANCE_H
#define CHESSINSTANCE_H

#include <functional>
#include <map>
#include <string>

class ChessInstance {
  public:
    using runFunction = std::function<void(void)>;

    explicit ChessInstance();
    ~ChessInstance();

    void run();
    void runRandom();
    void runAgainstRandom();
    void runWithChessData();
    void runWithChessDatabase();
    void runAgainstPeepo();
    void runAgainstDatabase();
    void runWithAnalyzer();

    void loadDB();

  private:
    std::map<std::string, runFunction> gameOptions;

    void printGameOptions();
};

#endif // CHESSINSTANCE_H
