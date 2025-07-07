#ifndef CHESSBOARDDRAW_H
#define CHESSBOARDDRAW_H

#include "ChessBoard.h"

struct ChessBoardDrawSettings {
    bool sysClear = false;
    bool useNerdFont = false;
};

class ChessBoardDraw {
  public:
    explicit ChessBoardDraw(ChessBoardDrawSettings settings = ChessBoardDrawSettings());

    void draw(const ChessBoard &board) const;

    ChessBoardDrawSettings &getSettings();

  private:
    ChessBoardDrawSettings settings;
};

#endif // CHESSBOARDDRAW_H
