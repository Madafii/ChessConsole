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

    void start();

    void run();
    void runRandom();
    void runAgainstRandom();
    void runDatabase();
    void runAgainstDatabase();
    void runAnalyzer();
    void runAgainstAnalyzer();
    void runWebInterface();

  private:
    std::map<std::string, runFunction> gameOptions;

    void printGameOptions();
};

#endif // CHESSINSTANCE_H
