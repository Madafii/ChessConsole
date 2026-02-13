#include "ChessPlayerConsoleHuman.h"

#include <iostream>
#include <optional>

#include "ChessBoardDraw.h"
#include "ChessInterface.h"

ChessPlayerConsoleHuman::ChessPlayerConsoleHuman(const ChessInterface &chessInterface, const ChessBoardDrawSettings &settings)
    : ChessPlayer(chessInterface), _boardDraw(settings) {}

ChessPlayerConsoleHuman::~ChessPlayerConsoleHuman() = default;

std::optional<std::string> ChessPlayerConsoleHuman::getNextMove() const {
    while (true) {
        auto fromInput = promptInput("enter a piece: ");
        if (!fromInput) return std::nullopt;

        auto highlightMoves = getValidatedMoves(*fromInput);
        if (!highlightMoves) continue;

        _boardDraw.draw(_chessInterface.getChessBoard(), *highlightMoves);

        while (true) {
            auto toInput = promptInput("enter a piece or tile to capture: ");
            if (!toInput) return std::nullopt;

            for (const ChessTile *tile : *highlightMoves) {
                if (toInput == tile->getPos()) return std::format("{}:{}", *fromInput, *toInput);
            }

            // check if input was a selection of another piece of current player
            auto newHighlightMoves = getValidatedMoves(*toInput);
            if (!newHighlightMoves) {
                std::cout << "can not move here, try another one" << std::endl;
                continue;
            }

            // set new from input variables
            fromInput = toInput;
            highlightMoves = newHighlightMoves;

            _boardDraw.draw(_chessInterface.getChessBoard(), *highlightMoves);
        }
    }
    return std::nullopt;
}

std::optional<std::string> ChessPlayerConsoleHuman::promptInput(const std::string &message) const {
    std::cout << message << std::endl;
    std::string input;
    std::cin >> input;
    if (input == "quit") return std::nullopt;
    return input;
}

std::optional<PieceTiles> ChessPlayerConsoleHuman::getValidatedMoves(std::string_view input) const {
    auto highlightMoves = _chessInterface.getPossibleMovesFromTile(input);
    if (highlightMoves == std::nullopt) {
        std::cout << "tile: " << input << " has none of your pieces" << std::endl;
        return std::nullopt;
    }
    if (highlightMoves->empty()) {
        std::cout << "this piece has no possible moves" << std::endl;
        return std::nullopt;
    }
    return highlightMoves;
}
