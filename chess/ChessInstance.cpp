#include "ChessInstance.h"
#include "ChessAnalyzer.h"
#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessData.h"
#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include "ChessMoveLogic.h"
#include "ChessPeepo.h"

#include <format>
#include <iostream>
#include <optional>
#include <random>

ChessInstance::ChessInstance() {
    gameOptions = {"normal", "random", "againstRandom", "data", "database", "peepo", "analyzer", "quit"};

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
            runWithChessDatabase();
        } else if (playOption == gameOptions[5]) {
            runAgainstPeepo();
        } else if (playOption == gameOptions[6]) {
            runWithAnalyzer();
        } else if (playOption == gameOptions[7]) {
            std::cout << "Quitting..." << std::endl;
            break;
        } else {
            printGameOptions();
        }
    }
}

ChessInstance::~ChessInstance() = default;

void ChessInstance::run() {
    ChessBoard chessBoard(false);
    ChessBoardDraw boardDraw;

    std::cout << "started a normal game..." << std::endl;
    std::string input;
    boardDraw.draw(chessBoard);
    while (true) {
        std::cin >> input;
        if (input == "quit") break;
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
        std::uniform_int_distribution<> distrFrom(0, static_cast<int>(allPieces.size()) - 1);
        std::string input;
        Pieces possMoves;
        while (true) {
            const int rndFromPiece = distrFrom(gen);
            const ChessTile *fromTile = allPieces.at(rndFromPiece);
            possMoves = ChessMoveLogic::getPossibleMoves(chessBoard, fromTile);
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
            Pieces allPieces = chessBoard.isWhitesTurn() ? chessBoard.getAllWhiteTiles() : chessBoard.getAllBlackTiles();
            std::uniform_int_distribution<> distrFrom(0, allPieces.size() - 1);
            Pieces possMoves;
            while (true) {
                const int rndFromPiece = distrFrom(gen);
                const ChessTile *fromTile = allPieces.at(rndFromPiece);
                possMoves = ChessMoveLogic::getPossibleMoves(chessBoard, fromTile);
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
    ChessBoardDraw chessDraw;
    ChessDatabaseInterface chessDB("chessMoves");

    // get the data
    ChessData data;
    const std::string filename = "../data/lichess/outData/lichess_db_standard.rated_2013-01.txt";
    data.readSimpleGames(filename);
    data.flushMovesToDB("chessMoves");

    while (true) {
        std::string inputNum, inputCol;
        std::cin >> inputNum;
        std::cin >> inputCol;

        int num = std::stoi(inputNum);
        bool white = inputCol == "w" ? true : false;

        auto move = chessDB.getMove({num, white}, 1);
        std::cout << ChessLinkedListMoves::getFullInfo(&move) << std::endl;
    }

    /*ChessBoard chessBoard(false);*/
    /*ChessBoardDraw chessDraw;*/
    /*ChessData data;*/
    /*const std::string filename = "/home/fpittermann/Documents/Projects/ChessConsole/data/lichessDatabase/outData/lichess_db_test.txt";*/
    /*/*const std::string filename = "../data/lichess/outData/lichess_db_standard.rated_2013-01_backup.txt";*/
    /*data.readSimpleGames(filename);*/
    /*ChessLinkedListMoves *moves = data.getMoves();*/
    /*moves->setMoveHead(moves->getMoveRoot()); // set it to the root before the game beginns*/
    /**/
    /*std::string input;*/
    /*chessDraw.draw(chessBoard);*/
    /*while (true) {*/
    /*    std::cin >> input;*/
    /*    if (input == "quit") break;*/
    /*    // handle game input*/
    /*    const GameState game_state = chessBoard.handleInput(input);*/
    /*    chessDraw.draw(chessBoard);*/
    /*    // get info for the next moves*/
    /*    Move *move = moves->getAtMove(input);*/
    /*    if (move == nullptr) {*/
    /*        std::cout << "no more suggested moves" << std::endl;*/
    /*    } else {*/
    /*        // set the head to the newly played move*/
    /*        moves->setMoveHead(move);*/
    /*        std::cout << moves->getInfoNextMoves() << std::endl;*/
    /*    }*/
    /*    if (game_state != GameState::IN_PROGRESS) {*/
    /*        break;*/
    /*    }*/
    /*}*/
}

void ChessInstance::runWithChessDatabase() {
    ChessBoard chessBoard;
    ChessBoardDraw chessDraw;
    ChessDatabaseInterface chessInterface("chessMoves");
}

void ChessInstance::runAgainstPeepo() {
    ChessBoard chessBoard;
    ChessBoardDraw chessDraw;

    // get the data
    ChessData data;
    const std::string filename = "../data/lichess/outData/lichess_db_standard.rated_2013-01.txt";
    data.readSimpleGames(filename);
    ChessLinkedListMoves *moves = data.getMoves();
    moves->setMoveHead(moves->getMoveRoot()); // set it to the root before the game beginns

    // your opponent
    ChessPeepo peepo(chessBoard, data);

    std::string input;
    chessDraw.draw(chessBoard);
    while (true) {
        std::cin >> input;
        if (input == "quit") break;

        // handle players move
        const GameState game_state = chessBoard.handleInput(input);
        // TODO: temp solution because head could get nullptr here so for now just do like that here
        if (moves->getMoveHead() != nullptr) {
            moves->setMoveHead(moves->getAtMove(input));
        }
        chessDraw.draw(chessBoard);

        // peepos move
        peepo.makeMostPlayedMove();
        chessDraw.draw(chessBoard);

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

void ChessInstance::runWithAnalyzer() {
    ChessBoard chessBoard(false);
    ChessAnalyzer boardAnalyzer(chessBoard);
    ChessBoardDraw boardDraw;

    std::cout << "started a game with the analyzer..." << std::endl;
    std::string input;
    boardDraw.draw(chessBoard);
    while (true) {
        std::cin >> input;
        if (input == "quit") break;
        const GameState game_state = chessBoard.handleInput(input);
        boardDraw.draw(chessBoard);
        auto getAttackerBoard = boardAnalyzer.getAttackedMatrix();
        auto getDefenderBoard = boardAnalyzer.getDefendedMatrix();
        std::cout << std::format("attacking total: {}\ndefending total: {}",
                                 ChessAnalyzer::boardMatrixSize(getAttackerBoard, chessBoard.isWhitesTurn()),
                                 ChessAnalyzer::boardMatrixSize(getDefenderBoard, chessBoard.isWhitesTurn()))
                  << std::endl;
        auto freePieces = boardAnalyzer.getFreePieces(getAttackerBoard, getDefenderBoard, chessBoard.isWhitesTurn());
        for (const auto *piece : freePieces) {
            std::cout << std::format("can take: {} at x:{} y:{}", piece->piece->getFullName(), piece->getX() + 1, piece->getY() + 1)
                      << std::endl;
        }
        std::cout << std::format("piece value of white: {}, black: {}", boardAnalyzer.getPieceValue(true),
                                 boardAnalyzer.getPieceValue(false))
                  << std::endl;
        std::cout << std::format("pawn advancement of white: {}, black; {}", boardAnalyzer.evalPawnStruct(true),
                                 boardAnalyzer.evalPawnStruct(false))
                  << std::endl;
        std::cout << std::format("the kings protection score is: {}", boardAnalyzer.evalKingProtection(true)) << std::endl;
        std::cout << std::format("the evaluated result of this board for the current player is: {}",
                                 boardAnalyzer.evalCurrPosition(chessBoard.isWhitesTurn()))
                  << std::endl;
        auto bestMovesSorted = boardAnalyzer.getBestEvalMoves(1);
        std::cout << std::format("recommended next move is: {}, which has the evaluation of: {}", bestMovesSorted.at(0).second,
                                 bestMovesSorted.at(0).first)
                  << std::endl;

        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}
