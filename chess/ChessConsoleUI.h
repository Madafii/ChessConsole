#pragma once

#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessUI.h"

class ChessConsoleUI : public ChessUI {
  public:
    ChessConsoleUI(PlayerType playerWhite, PlayerType playerBlack, const ChessBoardDrawSettings &settings = ChessBoardDrawSettings());

    void start() override;

  private:
    ChessBoardDraw _boardDraw;

    std::optional<std::string> inputLoop();
    bool doMove(std::string_view inputMove);
};
