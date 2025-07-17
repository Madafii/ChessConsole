#include "ChessInterface.h"
#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessPiece.h"

#include <iostream>

ChessInterface::ChessInterface() : chessLogic(ChessBoard()), chessBoard(chessLogic.getChessBoard()), chessDraw(ChessBoardDraw()) {}

// handles input usually by a user with checks for wrong inputs
GameState ChessInterface::handleInput(const std::string_view input) {
    const std::string color = chessBoard.isWhitesTurn() ? "white " : "black ";
    if (input == "resign") {
        std::cout << color << "resigned" << std::endl;
        return GameState::WON;
    }
    if (input == "draw") {
        std::cout << color << "is offering a draw. Do you accept? Y or N" << std::endl;
        std::string answer;
        std::cin >> answer;
        if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y") {
            // every answer besides this is a no.
            return GameState::DRAW;
        }
        return GameState::IN_PROGRESS;
    }
    // should be a move input
    return handleMoveInput(input);
}

// handles a correct input with before and after checks if moves are possible
GameState ChessInterface::handleMoveInput(const std::string_view input) {
    // TODO: check here if pawnChangeTo is needed for input
    const auto [inputMove, pawnChangeTo] = splitMoveInput(input);

    // get the move tiles from the input
    const auto moveTilePair = getMoveTilesFromInput(inputMove);
    if (!moveTilePair) {
        std::cout << "ChessBoard::handleMoveInput: invalid input: " << inputMove << std::endl;
        return GameState::IN_PROGRESS;
    }
    ChessTile &fromTile = *moveTilePair->first;
    ChessTile &toTile = *moveTilePair->second;

    if (!chessLogic.isInputMovePossible(fromTile, toTile)) {
        std::cout << "ChessBoard::handleMoveInput: move is not possible: " << inputMove << std::endl;
        return GameState::IN_PROGRESS;
    }
    chessBoard.move(fromTile, toTile, pawnChangeTo);

    return checkGameState();
}

/// directly make a move without any checks if it is a illegal move or not. For reading large databases with correct
/// input
/// @param input the input move in from:to format
/// @param enPassant if a enPassant is possible during this move
void ChessInterface::handleMoveInputNoChecks(const std::string_view input, const bool enPassant) {
    chessBoard.setEnPassantPossible(enPassant);
    auto [inputMove, pawnChangeTo] = splitMoveInput(input);

    // get the tiles directly
    inputMove[1] = static_cast<char>(inputMove[1] - 1);
    inputMove[4] = static_cast<char>(inputMove[4] - 1);
    ChessTile &fromTile = chessBoard.getTileAt(inputMove.substr(0, 2));
    ChessTile &toTile = chessBoard.getTileAt(inputMove.substr(3));

    chessBoard.move(fromTile, toTile, pawnChangeTo);
}

PiecePair ChessInterface::getMoveTilesFromInput(const std::string_view input) {
    if (input.length() != 5) {
        std::cout << "ChessBoard::getMoveTilesFromInput: wrong input. Length should be 5." << std::endl;
        return std::nullopt;
    }
    // get position strings from the from:to format input
    std::string subStrFrom{input.substr(0, 2)};
    std::string subStrTo{input.substr(3)};
    subStrFrom[1] = static_cast<char>(subStrFrom[1] - 1);
    subStrTo[1] = static_cast<char>(subStrTo[1] - 1);
    if (!ChessBoard::validTilePos(subStrFrom) || !ChessBoard::validTilePos(subStrTo)) {
        std::cout << "ChessBoard::getMoveTilesFromInput: tile coords out of board range." << std::endl;
        return std::nullopt;
    }
    ChessTile *fromTile = &chessBoard.getTileAt(subStrFrom);
    ChessTile *toTile = &chessBoard.getTileAt(subStrTo);
    if (fromTile->hasPiece(ChessPieceType::NONE) || fromTile->hasWhitePiece() != chessBoard.isWhitesTurn()) {
        std::cout << "ChessBoard::getMoveTilesFromInput: trying to move a piece from the opponent or no piece" << std::endl;
        return std::nullopt;
    }
    return std::make_optional(std::make_pair(fromTile, toTile));
}

GameState ChessInterface::checkGameState() {
    if (chessLogic.isDraw()) {
        std::cout << "this game is a draw!" << std::endl;
        return GameState::DRAW;
    }
    if (chessLogic.isKingCheckmate()) {
        std::cout << "you won the game!" << std::endl;
        return GameState::WON;
    }
    return GameState::IN_PROGRESS;
}
