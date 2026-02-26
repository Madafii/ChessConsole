#include "ChessInstance.h"
#include "ChessBoardDraw.h"
#include "ChessConsoleUI.h"
#include "ChessInterface.h"
#include "ChessPlayerAnalyzer.h"
#include "ChessPlayerConsoleHuman.h"
#include "ChessPlayerDB.h"
#include "ChessPlayerRnd.h"

#include <format>
#include <iostream>
#include <memory>
#include <optional>

ChessInstance::ChessInstance() {
    // all game options
    gameOptions.emplace("normal", [this] { run(); });
    gameOptions.emplace("random", [this] { runRandom(); });
    gameOptions.emplace("arandom", [this] { runAgainstRandom(); });
    gameOptions.emplace("database", [this] { runDatabase(); });
    gameOptions.emplace("adatabase", [this] { runAgainstDatabase(); });
    gameOptions.emplace("analyzer", [this] { runAnalyzer(); });
    gameOptions.emplace("aanalyzer", [this] { runAgainstAnalyzer(); });
    gameOptions.emplace("web", [this] { runWebInterface(); });
}

void ChessInstance::start() {
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
    ChessBoardDrawSettings settings(true, true);
    ChessConsoleUI cc([&settings](const ChessInterface &chessInterface) {
        return std::make_unique<ChessPlayerConsoleHuman>(chessInterface, settings);
    }, [&settings](const ChessInterface &chessInterface) {
        return std::make_unique<ChessPlayerConsoleHuman>(chessInterface, settings);
    }, settings);
    cc.start();
}

void ChessInstance::runRandom() {
    ChessBoardDrawSettings settings(false, true);
    ChessConsoleUI cc([](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerRnd>(chessInterface); },
                      [](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerRnd>(chessInterface); }, settings);
    cc.start();
}

void ChessInstance::runAgainstRandom() {
    ChessBoardDrawSettings settings(false, true);
    ChessConsoleUI cc([&settings](const ChessInterface &chessInterface) {
        return std::make_unique<ChessPlayerConsoleHuman>(chessInterface, settings);
    }, [](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerRnd>(chessInterface); }, settings);
    cc.start();
}

void ChessInstance::runDatabase() {
    ChessBoardDrawSettings settings(false, true);
    ChessConsoleUI cc([](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerDB>(chessInterface); },
                      [](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerDB>(chessInterface); }, settings);
    cc.start();
}

void ChessInstance::runAgainstDatabase() {
    ChessBoardDrawSettings settings(false, true);
    ChessConsoleUI cc([&settings](const ChessInterface &chessInterface) {
        return std::make_unique<ChessPlayerConsoleHuman>(chessInterface, settings);
    }, [](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerDB>(chessInterface); }, settings);
    cc.start();
}

void ChessInstance::runAnalyzer() {
    ChessBoardDrawSettings settings(false, true);
    ChessConsoleUI cc([](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerAnalyzer>(chessInterface); },
                      [](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerAnalyzer>(chessInterface); }, settings);
    cc.start();
}

void ChessInstance::runAgainstAnalyzer() {
    ChessBoardDrawSettings settings(false, true);
    ChessConsoleUI cc([&settings](const ChessInterface &chessInterface) {
        return std::make_unique<ChessPlayerConsoleHuman>(chessInterface, settings);
    }, [](const ChessInterface &chessInterface) { return std::make_unique<ChessPlayerAnalyzer>(chessInterface); }, settings);
    cc.start();
}

void ChessInstance::runWebInterface() {
    std::cout << "-------------------started web interface-------------------" << std::endl;

    ChessInterface chessInterface;

    while (true) {
        // input from piece
        std::string fromInput;
        std::cin >> fromInput;

        // give possible moves options
        auto possInputs = chessInterface.getPossibleMovesFromTile(fromInput);
        if (!possInputs) {
            std::cout << std::endl;
            continue;
        }
        for (const auto &input : *possInputs) {
            std::cout << input->getPos() << " ";
        }

        std::string toInput;
        std::cin >> toInput;

        for (const ChessTile *tile : *possInputs) {
            if (toInput == tile->getPos()) {
                chessInterface.handleMoveInput(std::format("{}:{}", fromInput, toInput));
                continue;
            }
        }
    }
}

void ChessInstance::printGameOptions() {
    std::cout << "The options are: " << std::endl;
    for (const auto &option : gameOptions) {
        std::cout << "\t" << option.first << std::endl;
    }
}
