//
// Created by fpittermann on 10/5/24.
//

#include "ChessInstance.h"
#include "ChessData.h"
#include <iostream>
#include <random>

enum class GameOptions { NORMAL, RANDOM, AGAINST_RANDOM };

ChessInstance::ChessInstance() {
    chessBoard = new ChessBoard();

    std::cout << "select the game you want to play: " << std::endl;
    std::string playOption;
    while (true) {
        std::cin >> playOption;
        if (playOption == "normal") {
            run();
        } else if (playOption == "random") {
            runRandom();
        } else if (playOption == "againstRandom") {
            std::string color;
            std::cout << "which color do you want to start as? white or black?" << std::endl;
            do {
                std::cin >> color;
            } while (color != "white" && color != "black");
            runAgainstRandom(color == "white");
        } else if (playOption == "data") {
            runWithChessData();
        } else if (playOption == "quit") {
            std::cout << "Quitting..." << std::endl;
            break;
        } else {
            std::cout << "The options are: normal or random or against Random" << std::endl;
        }
    }
}

ChessInstance::~ChessInstance() { delete chessBoard; }

void ChessInstance::run() {
    std::cout << "started a normal game";
    std::string input;
    while (true) {
        std::cin >> input;
        if (input == "quit")
            break;
        const GameState game_state = chessBoard->handleInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

void ChessInstance::runRandom() {
    std::random_device rd;
    std::mt19937 gen(rd());
    while (true) {
        Pieces allPieces = chessBoard->isWhitesTurn() ? chessBoard->getAllWhiteTiles()
                                                      : chessBoard->getAllBlackTiles();
        std::uniform_int_distribution<> distrFrom(0, allPieces.size() - 1);
        std::string input;
        Pieces possMoves;
        while (true) {
            const int rndFromPiece = distrFrom(gen);
            const ChessTile *fromTile = allPieces.at(rndFromPiece);
            possMoves = chessBoard->getPossibleMoves(fromTile);
            if (possMoves.size() != 0) {
                input += fromTile->getMove() + ":";
                break;
            }
        }
        std::uniform_int_distribution<> distrTo(0, possMoves.size() - 1);
        const int rndToPiece = distrTo(gen);
        const ChessTile *toTile = possMoves.at(rndToPiece);
        input += toTile->getMove();
        const GameState game_state = chessBoard->handleMoveInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

void ChessInstance::runAgainstRandom(const bool white) {
    std::random_device rd;
    std::mt19937 gen(rd());
    while (true) {
        std::string input;
        if (chessBoard->isWhitesTurn() == white) {
            std::cin >> input;
            if (input == "quit") {
                return;
            }
        } else {
            Pieces allPieces = chessBoard->isWhitesTurn() ? chessBoard->getAllWhiteTiles()
                                                          : chessBoard->getAllBlackTiles();
            std::uniform_int_distribution<> distrFrom(0, allPieces.size() - 1);
            Pieces possMoves;
            while (true) {
                const int rndFromPiece = distrFrom(gen);
                const ChessTile *fromTile = allPieces.at(rndFromPiece);
                possMoves = chessBoard->getPossibleMoves(fromTile);
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
        const GameState game_state = chessBoard->handleMoveInput(input);
        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

void ChessInstance::runWithChessData() {
    ChessData data;
    const std::string filename = "/home/fpittermann/Documents/Projects/ChessConsole/data/lichessDatabase/outData/lichess_db_standard.rated_2013-01.txt";
    data.readSimpleGames(filename);
}
