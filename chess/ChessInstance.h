#ifndef CHESSINSTANCE_H
#define CHESSINSTANCE_H

#include "ChessInterface.h"
#include "ChessBoardDraw.h"
#include <functional>
#include <map>
#include <optional>
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
    void runWebInterface();

    void loadDB();

  private:
    std::map<std::string, runFunction> gameOptions;

    void printGameOptions();
    // starts a loop to get a move input from the user. Takes two inputs, first from -> showing options -> the to
    std::optional<std::string> inputLoop(ChessInterface &chessInterface, const ChessBoardDraw &chessDraw);
};

#endif // CHESSINSTANCE_H
