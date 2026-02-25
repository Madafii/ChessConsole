#include "ChessInterface.h"
#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessPiece.h"

#include <iostream>
#include <optional>

ChessInterface::ChessInterface() : chessBoard(ChessBoard()), chessLogic(chessBoard) {}

ChessInterface::ChessInterface(const ChessBoard &board) : chessBoard(board), chessLogic(chessBoard) {}

ChessInterface &ChessInterface::operator=(const ChessInterface &other) {
    chessBoard = other.chessBoard;
    chessDraw = other.chessDraw;
    return *this;
}

// handles input usually by a user with checks for wrong inputs
std::optional<GameState> ChessInterface::handleInput(const std::string_view input) {
    const std::string color = chessBoard.isWhitesTurn() ? "white " : "black ";
    if (input == "resign") {
        std::cout << color << "resigned" << std::endl;
        return {GameState::Won};
    }
    if (input == "draw") {
        std::cout << color << "is offering a draw. Do you accept? Y or N" << std::endl;
        std::string answer;
        std::cin >> answer;
        if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y") {
            // every answer besides this is a no.
            return {GameState::Draw};
        }
        return {GameState::InProgress};
    }
    // should be a move input
    return handleMoveInput(input);
}

// handles a correct input with before and after checks if moves are possible
std::optional<GameState> ChessInterface::handleMoveInput(const std::string_view input) {
    // TODO: check here if pawnChangeTo is needed for input
    const auto [inputMove, pawnChangeTo] = splitMoveInput(input);

    // get the move tiles from the input
    const auto moveTilePair = getMoveTilesFromInput(inputMove);
    if (!moveTilePair) {
        std::cout << "ChessBoard::handleMoveInput: invalid input: " << inputMove << std::endl;
        return std::nullopt;
    }
    ChessTile &fromTile = *moveTilePair->first;
    ChessTile &toTile = *moveTilePair->second;

    if (!chessLogic.isInputMovePossible(fromTile, toTile)) {
        std::cout << "ChessBoard::handleMoveInput: move is not possible: " << inputMove << std::endl;
        return std::nullopt;
    }
    chessBoard.move(fromTile, toTile, pawnChangeTo);
    chessLogic.resetCache();

    return {checkGameState()};
}

/// directly make a move without any checks if it is a illegal move or not. For reading large databases with correct
/// input
/// @param input the input move in from:to format
/// @param enPassant if a enPassant is possible during this move
void ChessInterface::handleMoveInputNoChecks(const std::string_view input) {
    auto [inputMove, pawnChangeTo] = splitMoveInput(input);

    // get the tiles directly
    inputMove[1] = static_cast<char>(inputMove[1] - 1);
    inputMove[4] = static_cast<char>(inputMove[4] - 1);
    ChessTile &fromTile = chessBoard.getTileAt(inputMove.substr(0, 2));
    ChessTile &toTile = chessBoard.getTileAt(inputMove.substr(3));

    chessBoard.move(fromTile, toTile, pawnChangeTo);
}

// nullopt, when anything but your own piece. 0 possMoves does not return nullopt
std::optional<PieceTiles> ChessInterface::getPossibleMovesFromTile(const std::string_view input) const {
    if (const auto fromTile = getMoveTileFromInput(input)) {
        return chessLogic.getLegalMoves(*fromTile);
    }
    return std::nullopt;
}

std::optional<ChessTile> ChessInterface::handleToInput(std::string_view input, const PieceTiles &possMoves) const {
    if (!checkInputLength(input, 2)) return std::nullopt;

    const std::string internalTo = getInternalInput(input);
    if (!checkValidTilePos({internalTo})) return std::nullopt;

    const ChessTile *toTile = &chessBoard.getTileAt(internalTo);
    for (const ChessTile *tile : possMoves) {
        if (toTile == tile) return {*tile};
    }
    return std::nullopt;
}

std::optional<ChessTile> ChessInterface::getMoveTileFromInput(const std::string_view input) const {
    if (!checkInputLength(input, 2)) return std::nullopt;

    // get position strings from the from:to format input
    const std::string internalFrom = getInternalInput(input);
    if (!checkValidTilePos({internalFrom})) return std::nullopt;

    const ChessTile &tile = chessBoard.getTileAt(internalFrom);
    if (!checkFromTile(tile)) return std::nullopt;
    return tile;
}

PiecePair ChessInterface::getMoveTilesFromInput(const std::string_view input) {
    if (!checkInputLength(input, 5)) return std::nullopt;

    // get position strings from the from:to format input
    const std::string internalFrom = getInternalInput(input.substr(0, 2));
    const std::string internalTo = getInternalInput(input.substr(3));
    if (!checkValidTilePos({internalFrom, internalTo})) return std::nullopt;

    ChessTile &fromTile = chessBoard.getTileAt(internalFrom);
    ChessTile &toTile = chessBoard.getTileAt(internalTo);
    if (!checkFromTile(fromTile)) return std::nullopt;

    return std::make_pair(&fromTile, &toTile);
}

GameState ChessInterface::checkGameState() {
    if (chessLogic.isDraw(chessBoard.isWhitesTurn())) {
        std::cout << "this game is a draw!" << std::endl;
        return GameState::Draw;
    }
    if (chessLogic.isKingCheckmate(chessBoard.isWhitesTurn())) {
        std::cout << "you won the game!" << std::endl;
        return GameState::Won;
    }
    return GameState::InProgress;
}

bool ChessInterface::checkInputLength(std::string_view input, size_t length) const {
    if (input.length() != length) {
        std::cout << std::format("length of the input should be {}", length) << std::endl;
        return false;
    }
    return true;
}

bool ChessInterface::checkValidTilePos(const std::vector<std::string_view> &moves) const {
    for (const std::string_view move : moves) {
        if (!ChessBoard::validTilePos(move)) {
            std::cout << "input not in range of the chess board" << std::endl;
            return false;
        }
    }
    return true;
}

bool ChessInterface::checkFromTile(const ChessTile &fromTile) const {
    if (fromTile.hasPiece(ChessPieceType::NONE) || fromTile.hasWhitePiece() != chessBoard.isWhitesTurn()) {
        std::cout << "select one of your own pieces" << std::endl;
        return false;
    }
    return true;
}
