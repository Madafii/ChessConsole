//
// Created by finnp on 10/26/24.
//

#ifndef CHESSUTILS_H
#define CHESSUTILS_H

#include <string>
#include "ChessBoard.h"

class ChessUtils {
public:
    static std::string convertPGNToMyInput(std::string input, ChessBoard &board, const bool &white);
};

#endif //CHESSUTILS_H
