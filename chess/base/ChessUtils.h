#ifndef CHESSUTILS_H
#define CHESSUTILS_H

#include "ChessBoard.h"
#include <string>

namespace ChessUtils {
    std::string convertPGNToMyInput(std::string input, const ChessMoveLogic &chessLogic, const bool &white);
};

#endif // CHESSUTILS_H
