#include "ChessInterface.h"
#include "ChessBoard.h"
#include "ChessBoardDraw.h"
#include "ChessPiece.h"

#include <iostream>

ChessInterface::ChessInterface() : chessBoard(ChessBoard()), chessDraw(ChessBoardDraw()), chessLogic(chessBoard) {}

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
    char pawnChangeTo = '0'; // 0 is for no pawn change
    // extra check for instant pawn change
    if (input.length() == 7) {
        pawnChangeTo = input[6];
    }
    const std::string inputMove(input.substr(0, 5));

    // get the move tiles from the input
    const auto moveTilePair = getMoveTilesFromInput(inputMove);
    if (!moveTilePair) {
        std::cout << "ChessBoard::handleMoveInput: invalid input: " << inputMove << std::endl;
        return GameState::IN_PROGRESS;
    }
    ChessTile &fromTile = *moveTilePair->first;
    ChessTile &toTile = *moveTilePair->second;

    if (!chessLogic.isInputMovePossible(fromTile, toTile)) return GameState::IN_PROGRESS;
    // make the move
    chessBoard.move(fromTile, toTile);

    // after move checks should always be called after board has a new state
    const GameState game_state = afterMoveChecks(toTile, pawnChangeTo);
    return game_state;
}

/// directly make a move without any checks if it is a illegal move or not. For reading large databases with correct
/// input
/// @param input the input move in from:to format
/// @param enPassant if a enPassant is possible during this move
void ChessInterface::handleMoveInputNoChecks(const std::string_view input, const bool enPassant) {
    chessBoard.setEnPassantPossible(enPassant);
    char pawnChangeTo = '0'; // 0 is for no pawn change
    // extra check for instant pawn change
    if (input.length() == 7) {
        pawnChangeTo = input[6];
    }
    std::string inputMove(input.substr(0, 5));

    // get the tiles directly
    inputMove[1] = static_cast<char>(inputMove[1] - 1);
    inputMove[4] = static_cast<char>(inputMove[4] - 1);
    ChessTile &fromTile = chessBoard.getTileAt(inputMove.substr(0, 2));
    ChessTile &toTile = chessBoard.getTileAt(inputMove.substr(3));

    // make the move
    chessBoard.move(fromTile, toTile);

    // after move checks
    afterMoveChecks(toTile, pawnChangeTo);
}

// TODO improve if I want to with better getAll.. but for single pieces not all
// of them
Pieces ChessInterface::getAllPossibleMovesPiece(const bool white, const ChessPieceType piece) {
    Pieces allPossibleMoves;
    const Pieces colorPieceTiles = white ? chessBoard.getAllWhiteTiles() : chessBoard.getAllBlackTiles();
    for (const ChessTile *tile : colorPieceTiles) {
        if (tile->getPiece().getType() == piece) {
            ChessBoard::mergePossVec(allPossibleMoves, chessLogic.getPossibleMoves(*tile));
        }
    }
    return allPossibleMoves;
}

GameState ChessInterface::afterMoveChecks(ChessTile &toTile, const char pawnToPiece) {
    if (chessBoard.getEnPassantMarker() != chessBoard.isWhitesTurn()) {
        chessBoard.setEnPassantPossible(false);
        chessBoard.resetLastDoublePawnMove();
    }
    if (toTile.hasPiece(ChessPieceType::PAWN) && (toTile.getY() == 0 || toTile.getY() == 7)) {
        pawnWon(toTile, pawnToPiece);
    }
    // sometimes want to skip these for performance
    if (doAfterMoveChecks) {
        if (chessLogic.isDraw()) {
            std::cout << "this game is a draw!" << std::endl;
            return GameState::DRAW;
        }
        if (chessLogic.isKingCheckmate()) {
            std::cout << "you won the game!" << std::endl;
            return GameState::WON;
        }
    }

    chessBoard.addToGameHistory(chessBoard.getStringFromBoard());
    chessBoard.setTurn(!chessBoard.isWhitesTurn());
    // possibleMovesCache.clear();
    return GameState::IN_PROGRESS;
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

void ChessInterface::pawnWon(ChessTile &pawnTile, const char pawnToPiece) const {
    if (doAfterMoveChecks) std::cout << "Your pawn reached the end what should it become? (Q, R, B, N)" << std::endl;
    char newPawnTyp;
    if (pawnToPiece == '0')
        std::cin >> newPawnTyp;
    else
        newPawnTyp = pawnToPiece;
    while (!(newPawnTyp == 'Q' || newPawnTyp == 'R' || newPawnTyp == 'B' || newPawnTyp == 'N')) {
        std::cout << "That is not a valid type try another one: " << std::endl;
        std::cin >> newPawnTyp;
    }
    const bool white = pawnTile.getPiece().isWhite();
    switch (newPawnTyp) {
        case 'Q':
            pawnTile.changePiece(ChessPiece(ChessPieceType::QUEEN, white));
            break;
        case 'R':
            pawnTile.changePiece(ChessPiece(ChessPieceType::ROOK, white));
            break;
        case 'B':
            pawnTile.changePiece(ChessPiece(ChessPieceType::BISHOP, white));
            break;
        case 'N':
            pawnTile.changePiece(ChessPiece(ChessPieceType::KNIGHT, white));
            break;
        default:
            std::cout << "you broke the game f u" << std::endl;
            break;
    }
}
