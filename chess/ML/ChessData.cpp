#include "ChessData.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "ChessBoard.h"
#include "ChessUtils.h"

ChessData::ChessData() : movesLinkedList(std::make_unique<ChessLinkedListMoves>()) {}

void ChessData::readSimpleGames(const std::string &filename) {
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // file reading
            std::istringstream iss(line);
            std::string pgnMove;

            // get the game result for both colors
            std::string result;
            iss >> result;
            RESULT gameResultWhite, gameResultBlack;
            if (result == "1-0") {
                gameResultWhite = RESULT::WIN;
                gameResultBlack = RESULT::LOSE;
            } else if (result == "0-1") {
                gameResultWhite = RESULT::LOSE;
                gameResultBlack = RESULT::WIN;
            } else {
                gameResultWhite = RESULT::DRAW;
                gameResultBlack = RESULT::DRAW;
            }

            iss >> pgnMove; // skip second input

            // play the game
            ChessBoard board;
            bool whitesTurn = true;
            std::string boardMove, boardStr;

            while (iss >> pgnMove) {
                boardMove = ChessUtils::convertPGNToMyInput(pgnMove, board, whitesTurn);
                board.handleMoveInput(boardMove);
                boardStr = board.getStringFromBoard();
                movesLinkedList->addMove(boardStr, pgnMove, boardMove, whitesTurn ? gameResultWhite : gameResultBlack,
                                         whitesTurn);
                whitesTurn = !whitesTurn;
            }

            // set the head back to the root for the next game
            movesLinkedList->setMoveHead(movesLinkedList->getMoveRoot());
        }
        file.close();
    } else {
        std::cerr << "unable to open the file: " << filename << std::endl;
    }
    std::cout << "done reading all the data" << std::endl;
}

ChessLinkedListMoves *ChessData::getMoves() const { return movesLinkedList.get(); }

// example data
/*1-0 : e4 e6 d4 b6 a3 Bb7 Nc3 Nh6 Bxh6 gxh6 Be2 Qg5 Bg4 h5 Nf3 Qg6 Nh4 Qg5 Bxh5
 * Qxh4 Qf3 Kd8 Qxf7 Nc6 Qe8#*/
/*1-0 : d4 d5 Nf3 Nf6 e3 Bf5 Nh4 Bg6 Nxg6 hxg6 Nd2 e6 Bd3 Bd6 e4 dxe4 Nxe4 Rxh2
 * Ke2 Rxh1 Qxh1 Nc6 Bg5 Ke7 Qh7 Nxd4+ Kd2 Qe8 Qxg7 Qh8 Bxf6+ Kd7 Qxh8 Rxh8
 * Bxh8*/
