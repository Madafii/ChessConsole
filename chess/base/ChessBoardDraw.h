#ifndef CHESSBOARDDRAW_H
#define CHESSBOARDDRAW_H

#include <map>
#include "ChessBoard.h"

struct ChessBoardDrawSettings {
    bool sysClear = false;
    bool useNerdFont = false;
};

class ChessBoardDraw {
  public:
    explicit ChessBoardDraw(ChessBoardDrawSettings settings = ChessBoardDrawSettings());

    void draw(const ChessBoard &board) const;
    void drawHighlight(const ChessBoard &board, const Pieces &highlightTiles) const;

    ChessBoardDrawSettings &getSettings() { return _settings; }

  private:
    ChessBoardDrawSettings _settings;
    // TODO: kinda want to introduce styles which can be selected in the _settings variable
    std::string _backgroundColorWhite = "\x1b[48;5;0m"; // white
    std::string _backgroundColorBlack = "\x1b[48;5;231m"; // black
    std::string _pieceColorWhite = "\x1b[38;5;22m"; // green
    std::string _pieceColorBlack = "\x1b[38;5;126m"; // magenta
    std::string _defaultColor = "\x1b[0m"; // default terminal background color
    std::map<char, std::string> _whiteNerdFontPieces {
        {'K',""}, {'Q',""}, {'N',""}, {'P',""}, {'R', ""}, {'B', ""}
    };
    std::map<char, std::string> _blackNerdFontPieces {
        {'K',"󰡗"}, {'Q',"󰡚"}, {'N',"󰡘"}, {'P',"󰡙"}, {'R', "󰡛"}, {'B', "󰡜"}
    };
};

#endif // CHESSBOARDDRAW_H
