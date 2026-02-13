#ifndef CHESSBOARDDRAW_H
#define CHESSBOARDDRAW_H

#include "ChessBoard.h"
#include <map>

struct ChessBoardDrawSettings {
    bool sysClear = false;
    bool useNerdFont = false;
};

class ChessBoardDraw {
  public:
    explicit ChessBoardDraw(ChessBoardDrawSettings settings = ChessBoardDrawSettings());

    void draw(const ChessBoard &board, const PieceTiles &highlightTiles = {}) const;

    ChessBoardDrawSettings &getSettings() { return _settings; }

  private:
    ChessBoardDrawSettings _settings;
    // TODO: kinda want to introduce styles which can be selected in the _settings variable

    // ANSI escape sequences using the xterm 256-color palette.
    //
    // Format:
    //   \x1b[38;5;Xm  → Set foreground (text) color to color index X
    //   \x1b[48;5;Xm  → Set background color to color index X
    //   \x1b[0m       → Reset all colors and styles to terminal default
    //
    // Color indices follow the ANSI 256-color standard:
    //   0–15   : Basic + bright colors
    //   16–231 : 6×6×6 RGB color cube
    //   232–255: Grayscale ramp
    //
    std::string _backgroundColorWhite = "\x1b[48;5;231m";         // white
    std::string _backgroundColorBlack = "\x1b[48;5;0m";           // black
    std::string _backgroundColorHighlightWhite = "\x1b[48;5;229m"; // yellow
    std::string _backgroundColorHighlightBlack = "\x1b[48;5;8m"; // yellow
    std::string _pieceColorWhite = "\x1b[38;5;22m";               // green
    std::string _pieceColorBlack = "\x1b[38;5;126m";              // magenta
    std::string _defaultColor = "\x1b[0m";                        // default terminal background color
    static inline std::map<char, std::string> _whiteNerdFontPieces{{'K', ""}, {'Q', ""}, {'N', ""},
                                                                   {'P', ""}, {'R', ""}, {'B', ""}};
    static inline std::map<char, std::string> _blackNerdFontPieces{{'K', "󰡗"}, {'Q', "󰡚"}, {'N', "󰡘"},
                                                                   {'P', "󰡙"}, {'R', "󰡛"}, {'B', "󰡜"}};

    bool isHighlighted(int x, int y, const PieceTiles &highlightTiles) const;
};

#endif // CHESSBOARDDRAW_H
