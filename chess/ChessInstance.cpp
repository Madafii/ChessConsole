#include "ChessInstance.h"
#include "ChessData.h"
#include "ChessBoardDraw.h"
#include <iostream>
#include <random>

ChessInstance::ChessInstance() {
    gameOptions = {"normal", "random", "againstRandom", "data", "quit"};

    std::cout << "Select the game you want to play: " << std::endl;
    printGameOptions();
    std::string playOption;
    while (true) {
        std::cin >> playOption;
        if (playOption == gameOptions[0]) {
            run();
        } else if (playOption == gameOptions[1]) {
            runRandom();
        } else if (playOption == gameOptions[2]) {
            std::string color;
            std::cout << "which color do you want to start as? white or black?" << std::endl;
            do {
                std::cin >> color;
            } while (color != "white" && color != "black");
            runAgainstRandom(color == "white");
        } else if (playOption == gameOptions[3]) {
            runWithChessData();
        } else if (playOption == gameOptions[4]) {
            std::cout << "Quitting..." << std::endl;
            break;
        } else {
            printGameOptions();
        }
    }
}

ChessInstance::~ChessInstance() {}

void ChessInstance::run() {
    ChessBoard chessBoard;
    ChessBoardDraw boardDraw;

    std::cout << "started a normal game..." << std::endl;
    std::string input;
    boardDraw.draw(chessBoard);
    while (true) {
        std::cin >> input;
        if (input == "quit")
            break;
        const GameState game_state = chessBoard.handleInput(input);
        boardDraw.draw(chessBoard);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

void ChessInstance::runRandom() {
    ChessBoard chessBoard;
    std::random_device rd;
    std::mt19937 gen(rd());
    while (true) {
        Pieces allPieces = chessBoard.isWhitesTurn() ? chessBoard.getAllWhiteTiles() : chessBoard.getAllBlackTiles();
        std::uniform_int_distribution<> distrFrom(0, allPieces.size() - 1);
        std::string input;
        Pieces possMoves;
        while (true) {
            const int rndFromPiece = distrFrom(gen);
            const ChessTile *fromTile = allPieces.at(rndFromPiece);
            possMoves = chessBoard.getPossibleMoves(fromTile);
            if (possMoves.size() != 0) {
                input += fromTile->getMove() + ":";
                break;
            }
        }
        std::uniform_int_distribution<> distrTo(0, possMoves.size() - 1);
        const int rndToPiece = distrTo(gen);
        const ChessTile *toTile = possMoves.at(rndToPiece);
        input += toTile->getMove();
        const GameState game_state = chessBoard.handleMoveInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

void ChessInstance::runAgainstRandom(const bool white) {
    ChessBoard chessBoard;
    std::random_device rd;
    std::mt19937 gen(rd());
    while (true) {
        std::string input;
        if (chessBoard.isWhitesTurn() == white) {
            std::cin >> input;
            if (input == "quit") {
                return;
            }
        } else {
            Pieces allPieces =
                    chessBoard.isWhitesTurn() ? chessBoard.getAllWhiteTiles() : chessBoard.getAllBlackTiles();
            std::uniform_int_distribution<> distrFrom(0, allPieces.size() - 1);
            Pieces possMoves;
            while (true) {
                const int rndFromPiece = distrFrom(gen);
                const ChessTile *fromTile = allPieces.at(rndFromPiece);
                possMoves = chessBoard.getPossibleMoves(fromTile);
                if (possMoves.size() != 0) {
                    input += fromTile->getMove() + ":";
                    break;
                }
            }
            std::uniform_int_distribution<> distrTo(0, possMoves.size() - 1);
            const int rndToPiece = distrTo(gen);
            const ChessTile *toTile = possMoves.at(rndToPiece);
            input += toTile->getMove();
        }
        const GameState game_state = chessBoard.handleMoveInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

void ChessInstance::runWithChessData() {
    ChessBoard chessBoard;
    ChessData data;
    const std::string filename =
            "/home/fpittermann/Documents/Projects/ChessConsole/data/lichessDatabase/outData/lichess_db_test_50.txt";
    data.readSimpleGames(filename);
    ChessLinkedListMoves *moves = data.getMoves();
    moves->setMoveHead(moves->getMoveRoot()); // set it to the root before the game beginns

    std::string input;
    while (true) {
        std::cin >> input;
        if (input == "quit")
            break;
        // handle game input
        const GameState game_state = chessBoard.handleInput(input);
        // get info for the next moves
        Move *move = moves->getAtMove(input);
        if (move == nullptr) {
            std::cout << "no more suggested moves" << std::endl;
        } else {
            // set the head to the newly played move
            moves->setMoveHead(move);
            std::cout << moves->getInfoNextMoves() << std::endl;
        }
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}
inline void ChessInstance::printGameOptions() {
    std::cout << "The options are: " << std::endl;
    for (const std::string &option : gameOptions) {
        std::cout << "\t" << option << std::endl;
    }
}