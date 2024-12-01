#include "ChessData.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "ChessBoard.h"
#include "ChessMove.h"
#include "ChessUtils.h"

ChessData::ChessData() {}

void ChessData::readSimpleGames(const std::string &filename) {
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string pgnMove;
            /*std::vector<std::pair<std::string, std::string>> boardMovePair;*/

            // get the game result
            std::string result;
            iss >> result;
            RESULT gameResult;
            if (result == "1-0")
                gameResult = RESULT::WIN;
            else if (result == "0-1")
                gameResult = RESULT::LOSE;
            else
                gameResult = RESULT::DRAW;

            iss >> pgnMove; // skip second input

            // play the game and save moves and board
            ChessBoard board;
            bool whitesTurn = true;
            // get the first move for the root
            iss >> pgnMove; // the first move
            std::string boardMove = ChessUtils::convertPGNToMyInput(pgnMove, board, whitesTurn);
            board.handleMoveInput(boardMove);
            std::string boardStr = board.getStringFromBoard();
            const std::string key = ChessMove::createKey(whitesTurn, boardStr);

            // check if move already exists
            auto found =
                std::ranges::find_if(startMoves, [key](const std::unique_ptr<ChessMove> &move) {
                    return (move->getKey() == key);
                });
            if (found == startMoves.end()) {
                auto newMove = std::make_unique<ChessMove>(boardStr, pgnMove, boardMove, whitesTurn,
                                                           gameResult);
                startMoves.push_back(std::move(newMove));
                found = startMoves.end() - 1;
            } else {
                found->get()->addResult(gameResult);
            }
            whitesTurn = false;

            ChessMove *lastMove = found->get();
            while (iss >> pgnMove) {
                boardMove = ChessUtils::convertPGNToMyInput(pgnMove, board, whitesTurn);
                board.handleMoveInput(boardMove);
                boardStr = board.getStringFromBoard();
                lastMove = lastMove->addNext(boardStr, pgnMove, boardMove, gameResult, whitesTurn);
                whitesTurn = !whitesTurn;
            }
        }
        file.close();
    } else {
        std::cerr << "unable to open the file: " << filename << std::endl;
    }
    std::cout << "done reading all the data" << std::endl;
}

// example data
/*1-0 : e4 e6 d4 b6 a3 Bb7 Nc3 Nh6 Bxh6 gxh6 Be2 Qg5 Bg4 h5 Nf3 Qg6 Nh4 Qg5 Bxh5
 * Qxh4 Qf3 Kd8 Qxf7 Nc6 Qe8#*/
/*1-0 : d4 d5 Nf3 Nf6 e3 Bf5 Nh4 Bg6 Nxg6 hxg6 Nd2 e6 Bd3 Bd6 e4 dxe4 Nxe4 Rxh2
 * Ke2 Rxh1 Qxh1 Nc6 Bg5 Ke7 Qh7 Nxd4+ Kd2 Qe8 Qxg7 Qh8 Bxf6+ Kd7 Qxh8 Rxh8
 * Bxh8*/
