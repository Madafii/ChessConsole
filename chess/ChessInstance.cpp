#include "ChessInstance.h"
// #include "ChessAnalyzer.h"
#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessData.h"
#include "ChessDatabaseInterface.h"
#include "ChessInterface.h"
#include "ChessLinkedListMoves.h"
#include "ChessMoveLogic.h"
#include "ChessPeepo.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <iterator>
#include <optional>

ChessInstance::ChessInstance() {
    // all game options
    gameOptions.emplace("normal", [this] { run(); });
    gameOptions.emplace("random", [this] { runRandom(); }), gameOptions.emplace("againstRandom", [this] { runAgainstRandom(); }),
        gameOptions.emplace("data", [this] { runWithChessData(); }), gameOptions.emplace("database", [this] { runAgainstDatabase(); }),
        gameOptions.emplace("peepo", [this] { runAgainstPeepo(); }), gameOptions.emplace("analyzer", [this] { runWithAnalyzer(); }),
        gameOptions.emplace("loadDB", [this] { loadDB(); });

    std::cout << "Select the game you want to play: " << std::endl;
    printGameOptions();
    std::string playOption;
    while (true) {
        std::cin >> playOption;
        if (gameOptions.contains(playOption)) {
            gameOptions.at(playOption)();
        } else if (playOption == "quit") {
            std::cout << "Quitting..." << std::endl;
            break;
        } else {
            printGameOptions();
        }
    }
}

ChessInstance::~ChessInstance() = default;

void ChessInstance::run() {
    ChessInterface chessInterface;
    ChessBoardDraw boardDraw;

    std::cout << "started a normal game..." << std::endl;
    std::string input;
    boardDraw.draw(chessInterface.getChessBoard());
    while (true) {
        std::cin >> input;
        if (input == "quit") break;
        if (const auto game_state = chessInterface.handleInput(input)) {
            boardDraw.draw(chessInterface.getChessBoard());
            if (game_state.value() != GameState::IN_PROGRESS) {
                break;
            }
        }
    }
}

void ChessInstance::runRandom() {
    // ChessBoard chessBoard;
    // ChessMoveLogic chessLogic(chessBoard);
    // ChessInterface chessInterface;
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // while (true) {
    //     Pieces allPieces = chessBoard.isWhitesTurn() ? chessBoard.getAllWhiteTiles() : chessBoard.getAllBlackTiles();
    //     std::uniform_int_distribution<> distrFrom(0, static_cast<int>(allPieces.size()) - 1);
    //     std::string input;
    //     Pieces possMoves;
    //     while (true) {
    //         const int rndFromPiece = distrFrom(gen);
    //         const ChessTile *fromTile = allPieces.at(rndFromPiece);
    //         possMoves = chessLogic.getPossibleMoves(*fromTile);
    //         if (possMoves.size() != 0) {
    //             input += fromTile->getMove() + ":";
    //             break;
    //         }
    //     }
    //     std::uniform_int_distribution<> distrTo(0, possMoves.size() - 1);
    //     const int rndToPiece = distrTo(gen);
    //     const ChessTile *toTile = possMoves.at(rndToPiece);
    //     input += toTile->getMove();
    //     const GameState game_state = chessInterface.handleMoveInput(input);
    //     if (game_state != GameState::IN_PROGRESS) {
    //         break;
    //     }
    // }
}

void ChessInstance::runAgainstRandom() {
    // bool white = true;
    // ChessBoard chessBoard;
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // while (true) {
    //     std::string input;
    //     if (chessBoard.isWhitesTurn() == white) {
    //         std::cin >> input;
    //         if (input == "quit") {
    //             return;
    //         }
    //     } else {
    //         Pieces allPieces = chessBoard.isWhitesTurn() ? chessBoard.getAllWhiteTiles() : chessBoard.getAllBlackTiles();
    //         std::uniform_int_distribution<> distrFrom(0, allPieces.size() - 1);
    //         Pieces possMoves;
    //         while (true) {
    //             const int rndFromPiece = distrFrom(gen);
    //             const ChessTile *fromTile = allPieces.at(rndFromPiece);
    //             possMoves = ChessMoveLogic::getPossibleMoves(chessBoard, fromTile);
    //             if (possMoves.size() != 0) {
    //                 input += fromTile->getMove() + ":";
    //                 break;
    //             }
    //         }
    //         std::uniform_int_distribution<> distrTo(0, possMoves.size() - 1);
    //         const int rndToPiece = distrTo(gen);
    //         const ChessTile *toTile = possMoves.at(rndToPiece);
    //         input += toTile->getMove();
    //     }
    //     const GameState game_state = chessBoard.handleMoveInput(input);
    //     if (game_state != GameState::IN_PROGRESS) {
    //         break;
    //     }
    // }
}

void ChessInstance::runWithChessData() {
    ChessBoard chessBoard;
    ChessBoardDraw chessDraw;
    ChessDatabaseInterface chessDB("chessMoves");

    // get the data
    ChessData data;
    const std::string filename = "../data/lichess/outDataPGN/lichess_db_standard.rated_2013-01.txt";
    const std::string outFilename = "../data/lichess/outDataMoves/liches_db_rated_2013-01.txt";
    data.convertPGNToMoves(filename, outFilename);
    // data.readSimpleGames(filename);
    // data.flushMovesToDB("chessMoves");

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
    // ChessBoard chessBoard;
    // ChessBoardDraw chessDraw;
    //
    // // get the data
    // ChessData data;
    // const std::string filename = "../data/lichess/outData/lichess_db_standard.rated_2013-01.txt";
    // data.readSimpleGames(filename);
    // ChessLinkedListMoves *moves = data.getMoves();
    // moves->setMoveHead(moves->getMoveRoot()); // set it to the root before the game beginns
    //
    // // your opponent
    // ChessPeepo peepo(chessBoard, data);
    //
    // std::string input;
    // chessDraw.draw(chessBoard);
    // while (true) {
    //     std::cin >> input;
    //     if (input == "quit") break;
    //
    //     // handle players move
    //     const GameState game_state = chessBoard.handleInput(input);
    //     // TODO: temp solution because head could get nullptr here so for now just do like that here
    //     if (moves->getMoveHead() != nullptr) {
    //         moves->setMoveHead(moves->getAtMove(input));
    //     }
    //     chessDraw.draw(chessBoard);
    //
    //     // peepos move
    //     peepo.makeMostPlayedMove();
    //     chessDraw.draw(chessBoard);
    //
    //     if (game_state != GameState::IN_PROGRESS) {
    //         break;
    //     }
    // }
}

void ChessInstance::runAgainstDatabase() {
    ChessInterface chessInterface;
    const ChessBoard &chessBoard = chessInterface.getChessBoard();
    ChessBoardDraw chessDraw(ChessBoardDrawSettings(true, true));

    // your opponent
    ChessDatabaseInterface chessDB("chessMoves");

    GameState game_state = GameState::IN_PROGRESS;

    // setup
    table_pair gameDepth(0, false);
    int fromMoveId = 1; // is the from id for the empty board

    chessDraw.draw(chessBoard);
    while (true) {
        auto input = inputLoop(chessInterface, chessDraw);
        if (!input) break; // quit the game

        chessInterface.handleMoveInput(*input);

        // get oponents move id
        if (auto whiteMoveId = chessDB.getMoveIdOpt(gameDepth, fromMoveId, ChessLinkedListMoves::createData(*input, false))) {
            fromMoveId = *whiteMoveId;

            // increment after getting the id
            ++gameDepth;

            // get move from oponent and get best next move from db
            auto nextMoves = chessDB.getNextMoves(gameDepth, fromMoveId);

            // transform to pointer vector
            std::vector<MoveCompressed *> nextMovePtrs;
            nextMovePtrs.reserve(nextMoves.size());
            std::ranges::transform(nextMoves, std::back_inserter(nextMovePtrs), [](auto &move) { return &move.second; });

            // get the move string
            auto bestMove = ChessPeepo::getMostPlayedMove(nextMovePtrs);
            std::string dbMove = ChessLinkedListMoves::getMoveFromData(bestMove->data);
            std::cout << "the db makes the move: " << dbMove << std::endl;

            // make the move
            game_state = chessInterface.handleInput(dbMove).value();

            // increment turn
            fromMoveId = *chessDB.getMoveIdOpt(gameDepth, fromMoveId, bestMove->data);
            ++gameDepth;
        } else {
            // could not find the move so make a random move
            const std::string randomMove = ChessPeepo::getRandomInputMove(chessInterface);
            std::cout << "the db makes the random move: " << randomMove << std::endl;
            game_state = chessInterface.handleInput(randomMove).value();
        }

        chessDraw.draw(chessBoard);

        if (game_state != GameState::IN_PROGRESS) {
            break;
        }
    }
}

void ChessInstance::runWithAnalyzer() {
    //     ChessBoard chessBoard(false);
    //     ChessAnalyzer boardAnalyzer(chessBoard);
    //     ChessBoardDraw boardDraw;
    //
    //     std::cout << "started a game with the analyzer..." << std::endl;
    //     std::string input;
    //     boardDraw.draw(chessBoard);
    //     while (true) {
    //         std::cin >> input;
    //         if (input == "quit") break;
    //         const GameState game_state = chessBoard.handleInput(input);
    //         boardDraw.draw(chessBoard);
    //         auto getAttackerBoard = boardAnalyzer.getAttackedMatrix();
    //         auto getDefenderBoard = boardAnalyzer.getDefendedMatrix();
    //         std::cout << std::format("attacking total: {}\ndefending total: {}",
    //                                  ChessAnalyzer::boardMatrixSize(getAttackerBoard, chessBoard.isWhitesTurn()),
    //                                  ChessAnalyzer::boardMatrixSize(getDefenderBoard, chessBoard.isWhitesTurn()))
    //                   << std::endl;
    //         auto freePieces = boardAnalyzer.getFreePieces(getAttackerBoard, getDefenderBoard, chessBoard.isWhitesTurn());
    //         for (const auto *piece : freePieces) {
    //             std::cout << std::format("can take: {} at x:{} y:{}", piece->piece->getFullName(), piece->getX() + 1, piece->getY() + 1)
    //                       << std::endl;
    //         }
    //         std::cout << std::format("piece value of white: {}, black: {}", boardAnalyzer.getPieceValue(true),
    //                                  boardAnalyzer.getPieceValue(false))
    //                   << std::endl;
    //         std::cout << std::format("pawn advancement of white: {}, black; {}", boardAnalyzer.evalPawnStruct(true),
    //                                  boardAnalyzer.evalPawnStruct(false))
    //                   << std::endl;
    //         std::cout << std::format("the kings protection score is: {}", boardAnalyzer.evalKingProtection(true)) << std::endl;
    //         std::cout << std::format("the evaluated result of this board for the current player is: {}",
    //                                  boardAnalyzer.evalCurrPosition(chessBoard.isWhitesTurn()))
    //                   << std::endl;
    //         auto bestMovesSorted = boardAnalyzer.getBestEvalMoves(1);
    //         std::cout << std::format("recommended next move is: {}, which has the evaluation of: {}", bestMovesSorted.at(0).second,
    //                                  bestMovesSorted.at(0).first)
    //                   << std::endl;
    //
    //         if (game_state != GameState::IN_PROGRESS) {
    //             break;
    //         }
    //     }
}

void ChessInstance::loadDB() {}

void ChessInstance::printGameOptions() {
    std::cout << "The options are: " << std::endl;
    for (const auto &option : gameOptions) {
        std::cout << "\t" << option.first << std::endl;
    }
}

std::optional<std::string> ChessInstance::inputLoop(ChessInterface &chessInterface, const ChessBoardDraw &chessDraw) {
    while (true) {
        std::cout << "enter a piece: " << std::endl;
        std::string fromInput;
        std::cin >> fromInput;
        if (fromInput == "quit") return std::nullopt;

        auto highlightMoves = chessInterface.handleFromInput(fromInput);
        if (highlightMoves == std::nullopt) continue; // no moves possible from this point

        chessDraw.draw(chessInterface.getChessBoard(), *highlightMoves);

        while (true) {
            std::cout << "enter a piece or tile to capture: " << std::endl;
            std::string toInput;
            std::cin >> toInput;
            if (toInput == "quit") return std::nullopt;

            for (const ChessTile *tile : *highlightMoves) {
                if (toInput == tile->getMove()) return std::format("{}:{}", fromInput, toInput);
            }
            std::cout << "can not move to here, try another one" << std::endl;

            // check if input was a selection of another piece of current player
            auto newHighlightMoves = chessInterface.handleFromInput(toInput);
            if (newHighlightMoves == std::nullopt) continue; // no new piece got selected
            // set new from input variables
            fromInput = toInput;
            highlightMoves = newHighlightMoves;

            chessDraw.draw(chessInterface.getChessBoard(), *highlightMoves);
        }
    }
    return std::nullopt;
}
