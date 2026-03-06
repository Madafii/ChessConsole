#pragma once

#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessUI.h"

class ChessConsoleUI : public ChessUI {
  public:
    ChessConsoleUI(const PlayerFactory &playerWhite, const PlayerFactory &playerBlack,
                   const ChessBoardDrawSettings &settingsWhite = ChessBoardDrawSettings());

    void start() override;
    void startSteps(int steps);

  private:
    ChessBoardDraw _boardDraw;

    std::optional<std::string> inputLoop();
    bool doMove(std::string_view inputMove);
};
