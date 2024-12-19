#include "ChessBoard.h"
#include "ChessMoveLogic.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

ChessBoard::ChessBoard(const bool doAfterMoveChecks) : doAfterMoveChecks(doAfterMoveChecks) { initBoard(); }

ChessBoard::~ChessBoard() {}

void ChessBoard::initBoard() {
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Rook, true), 0, 0));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Knight, true), 1, 0));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Bishop, true), 2, 0));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Queen, true), 3, 0));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(King, true), 4, 0));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Bishop, true), 5, 0));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Knight, true), 6, 0));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Rook, true), 7, 0));
    for (int i = 0; i < boardWidth; i++) {
        board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Pawn, true), i, 1));
    }
    for (int i = 0; i < boardWidth * 4; i++) {
        board.push_back(std::make_unique<ChessTile>(nullptr, i % boardWidth, i / boardWidth + 2));
    }
    for (int i = 0; i < boardWidth; i++) {
        board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Pawn, false), i, 6));
    }
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Rook, false), 0, 7));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Knight, false), 1, 7));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Bishop, false), 2, 7));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Queen, false), 3, 7));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(King, false), 4, 7));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Bishop, false), 5, 7));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Knight, false), 6, 7));
    board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(Rook, false), 7, 7));
    // add initial board as first in the game history
    gameHistory.push_back(getStringFromBoard());
}

// handles input usually by a user with checks for wrong inputs
GameState ChessBoard::handleInput(const std::string_view input) {
    const std::string color = whitesTurn ? "white " : "black ";
    if (input == "resign") {
        std::cout << color << "won" << std::endl;
        return GameState::WON;
    }
    if (input == "draw") {
        std::cout << color << "is offering a draw. Do you accept? Y or N." << std::endl;
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
GameState ChessBoard::handleMoveInput(const std::string_view input) {
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
    ChessTile *fromTile = moveTilePair->first;
    ChessTile *toTile = moveTilePair->second;

    if (!ChessMoveLogic::isInputMovePossible(*this, fromTile, toTile)) return GameState::IN_PROGRESS;
    // make the move
    move(fromTile, toTile);

    // after move checks should always be called after board has a new state
    const GameState game_state = afterMoveChecks(toTile, pawnChangeTo);
    return game_state;
}

/// directly make a move without any checks if it is a illegal move or not. For reading large databases with correct
/// input
/// @param input the input move in from:to format
/// @param enPassant if a enPassant is possible during this move
void ChessBoard::handleMoveInputNoChecks(const std::string_view input, const bool enPassant) {
    enPassantPossibleLastMove = enPassant;
    char pawnChangeTo = '0'; // 0 is for no pawn change
    // extra check for instant pawn change
    if (input.length() == 7) {
        pawnChangeTo = input[6];
    }
    std::string inputMove(input.substr(0, 5));

    // get the tiles directly
    inputMove[1] = inputMove[1] - 1;
    inputMove[4] = inputMove[4] - 1;
    ChessTile *fromTile = getTileAt(inputMove.substr(0, 2));
    ChessTile *toTile = getTileAt(inputMove.substr(3));

    // make the move
    move(fromTile, toTile);

    // after move checks
    afterMoveChecks(toTile, pawnChangeTo);
}

// get a simple string of the current board
std::string ChessBoard::getStringFromBoard() {
    std::string outMoves;
    for (const auto &tile : board) {
        if (tile->piece == nullptr) {
            outMoves += "_";
            continue;
        }
        if (tile->piece->isWhite()) {
            // white pieces in lower case
            outMoves += static_cast<char>(std::tolower(tile->piece->getShortName()));
            continue;
        }
        // black pieces in upper case
        outMoves += tile->piece->getShortName();
    }
    // TODO: thinking of writing a separate function to know if a en Passant is possible
    if (enPassantPossibleLastMove) {
        outMoves += "1";
    }
    return outMoves;
}

Pieces ChessBoard::getAllWhiteTiles() const {
    Pieces whiteTiles;
    for (const auto &tile : board) {
        if (tile->piece == nullptr) continue;
        if (tile->piece->isWhite()) whiteTiles.push_back(tile.get());
    }
    return whiteTiles;
}

Pieces ChessBoard::getAllBlackTiles() const {
    Pieces blackTiles;
    for (const auto &tile : board) {
        if (tile->piece == nullptr) continue;
        if (!tile->piece->isWhite()) blackTiles.push_back(tile.get());
    }
    return blackTiles;
}

bool ChessBoard::isWhitesTurn() { return whitesTurn; }

// TODO improve if I want to with better getAll.. but for single pieces not all
// of them
Pieces ChessBoard::getAllPossibleMovesPiece(const bool white, const ChessPieceType piece) {
    Pieces allPossibleMoves;
    const Pieces colorPieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile : colorPieceTiles) {
        if (tile->piece->getType() == piece) {
            mergePossVec(allPossibleMoves, ChessMoveLogic::getPossibleMoves(*this, tile));
        }
    }
    return allPossibleMoves;
}

Pieces ChessBoard::getAllPiecesFor(const bool white, const ChessPieceType piece) const {
    Pieces pieces;
    const Pieces colorPieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (ChessTile *tile : colorPieceTiles) {
        if (tile->piece->getType() == piece) {
            pieces.push_back(tile);
        }
    }
    return pieces;
}

void ChessBoard::move(ChessTile *fromTile, ChessTile *toTile) {
    switch (fromTile->piece->getType()) {
        case Pawn:
            movePawn(fromTile, toTile);
            break;
        case Rook:
            moveRook(fromTile);
            break;
        case King:
            moveKing(fromTile, toTile);
        default:
            break;
    }

    // for optional or 100 moves draw rule
    if (toTile->piece != nullptr) {
        movesSinceLastCapture = 0;
    } else if (fromTile->piece->getType() == Pawn) {
        movesSinceLastCapture = 0;
    } else {
        movesSinceLastCapture++;
    }

    toTile->piece = std::move(fromTile->piece);
    gameHistory.push_back(getStringFromBoard());
}

void ChessBoard::movePawn(const ChessTile *fromTile, const ChessTile *toTile) {
    // for that special pawn movement check if pawn moved more than two tiles.
    if (abs(fromTile->getY() - toTile->getY()) >= 2) {
        doublePawnMoveAt = std::make_pair(toTile->getX(), toTile->getY());
        markTurnForEnPassant = whitesTurn; // mark for delete next turn
    }
    // that special move happened so extra rule with capturing
    if (fromTile->getX() != toTile->getX() && toTile->piece == nullptr) {
        const int whiteMove = whitesTurn ? -1 : 1;
        ChessTile *capturedPiece = getTileAt(toTile->getX(), toTile->getY() + whiteMove);
        capturedPiece->piece.reset();
    }
}

void ChessBoard::moveKing(const ChessTile *fromTile, const ChessTile *toTile) {
    // disables castling when the king is being moved
    if (fromTile->piece->isWhite() == true) {
        whiteRookMoved.first = true;
        whiteRookMoved.second = true;
    } else {
        blackRookMoved.first = true;
        blackRookMoved.second = true;
    }
    // do castling move for the rook
    if (abs(fromTile->getX() - toTile->getX()) >= 2) {
        const int x = toTile->getX();
        const int y = toTile->getY();
        ChessTile *rookTile;
        ChessTile *rookToTile;
        if (x < 4) {
            rookTile = getTileAt(0, y);
            rookToTile = getTileAt(3, y);
        } else {
            rookTile = getTileAt(7, y);
            rookToTile = getTileAt(5, y);
        }
        rookToTile->piece = std::move(rookTile->piece);
    }
}

inline void ChessBoard::moveRook(const ChessTile *fromTile) {
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    // just mark them as moved
    if (x == 0) {
        if (y == 0) whiteRookMoved.first = true;
        if (y == 7) blackRookMoved.first = true;
    }
    if (x == 7) {
        if (y == 0) whiteRookMoved.second = true;
        if (y == 7) blackRookMoved.second = true;
    }
}

ChessTile *ChessBoard::getTileAt(const std::string_view pos) const {
    if (pos.size() != 2) {
        std::cout << "ChessBoard::getTileAt: Wrong input length." << std::endl;
        return nullptr;
    }
    const char xText = pos[0];
    if (xText != std::clamp(xText, 'a', 'h')) return nullptr;
    const int x = ChessTile::mapXtoInt.at(xText);
    const int y = pos[1] - '0'; // is a trick to convert number char to int
    return getTileAt(x, y);
}

inline ChessTile *ChessBoard::getTileAt(const int x, const int y) const {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) return nullptr;
    return board[y * boardWidth + x].get();
}

void ChessBoard::pawnWon(ChessTile *pawnTile, const char pawnToPiece) const {
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
    const bool white = pawnTile->piece->isWhite();
    std::unique_ptr<ChessPiece> newPawn;
    switch (newPawnTyp) {
        case 'Q':
            newPawn = std::make_unique<ChessPiece>(Queen, white);
            break;
        case 'R':
            newPawn = std::make_unique<ChessPiece>(Rook, white);
            break;
        case 'B':
            newPawn = std::make_unique<ChessPiece>(Bishop, white);
            break;
        case 'N':
            newPawn = std::make_unique<ChessPiece>(Knight, white);
            break;
        default:
            std::cout << "you broke the game f u" << std::endl;
            break;
    }
    pawnTile->piece = std::move(newPawn);
}

GameState ChessBoard::afterMoveChecks(ChessTile *toTile, const char pawnToPiece) {
    if (markTurnForEnPassant != whitesTurn) {
        enPassantPossibleLastMove = false;
        doublePawnMoveAt = std::make_pair(-1, -1);
    }
    if (toTile->piece->getType() == Pawn && (toTile->getY() == 0 || toTile->getY() == 7)) {
        pawnWon(toTile, pawnToPiece);
    }
    // sometimes want to skip these for performance
    if (doAfterMoveChecks) {
        if (ChessMoveLogic::isDraw(*this)) {
            std::cout << "this game is a draw!" << std::endl;
            return GameState::DRAW;
        }
        if (ChessMoveLogic::isKingCheckmate(*this)) {
            std::cout << "you won the game!" << std::endl;
            return GameState::WON;
        }
    }
    whitesTurn = !whitesTurn;
    possibleMovesCache.clear();
    return GameState::IN_PROGRESS;
}

PiecePair ChessBoard::getMoveTilesFromInput(const std::string &input) const {
    if (input.length() != 5) {
        std::cout << "ChessBoard::getMoveTilesFromInput: Wrong input length, should be 5." << std::endl;
        return std::nullopt;
    }
    std::string subStrFrom = input.substr(0, 2);
    std::string subStrTo = input.substr(3);
    subStrFrom[1] = subStrFrom[1] - 1;
    subStrTo[1] = subStrTo[1] - 1;
    ChessTile *fromTile = getTileAt(subStrFrom);
    ChessTile *toTile = getTileAt(subStrTo);
    if (fromTile == nullptr || toTile == nullptr) {
        std::cout << "ChessBoard::getMoveTilesFromInput: there is no tile like that" << std::endl;
        return std::nullopt;
    }
    if (fromTile->piece.get() == nullptr || fromTile->piece->isWhite() != whitesTurn) {
        std::cout << "ChessBoard::getMoveTilesFromInput: trying to move a piece "
                     "from the opponent or no piece"
                  << std::endl;
        return std::nullopt;
    }
    return std::make_optional(std::make_pair(fromTile, toTile));
}

inline void ChessBoard::mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge) {
    possibleMoves.insert(possibleMoves.end(), possibleMovesMerge.begin(), possibleMovesMerge.end());
}
