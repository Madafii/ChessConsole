#include "ChessBoard.h"
#include "ChessMoveLogic.h"
#include "ChessPiece.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>

ChessBoard::ChessBoard(const bool doAfterMoveChecks) : doAfterMoveChecks(doAfterMoveChecks) { initBoard(); }

void ChessBoard::initBoard() {
    board[0] = ChessTile(ChessPiece(ChessPieceType::KING, true), 0, 0);
    board[1] = ChessTile(ChessPiece(ChessPieceType::KNIGHT, true), 1, 0);
    board[2] = ChessTile(ChessPiece(ChessPieceType::BISHOP, true), 2, 0);
    board[3] = ChessTile(ChessPiece(ChessPieceType::QUEEN, true), 3, 0);
    board[4] = ChessTile(ChessPiece(ChessPieceType::KING, true), 4, 0);
    board[5] = ChessTile(ChessPiece(ChessPieceType::BISHOP, true), 5, 0);
    board[6] = ChessTile(ChessPiece(ChessPieceType::KNIGHT, true), 6, 0);
    board[7] = ChessTile(ChessPiece(ChessPieceType::ROOK, true), 7, 0);
    for (int i = 0; i < boardWidth; i++) {
        board[i + boardWidth] = ChessTile(ChessPiece(ChessPieceType::PAWN, true), i, 1);
    }
    for (int i = 0; i < boardWidth * 4; i++) {
        board[i + 2 * boardWidth] = ChessTile(ChessPiece(ChessPieceType::NONE, true), i % boardWidth, i / boardWidth + 2);
    }
    for (int i = 0; i < boardWidth; i++) {
        board[i + 6 * boardWidth] = ChessTile(ChessPiece(ChessPieceType::PAWN, false), i, 6);
    }
    board[56] = ChessTile(ChessPiece(ChessPieceType::ROOK, false), 0, 7);
    board[57] = ChessTile(ChessPiece(ChessPieceType::KNIGHT, false), 1, 7);
    board[58] = ChessTile(ChessPiece(ChessPieceType::BISHOP, false), 2, 7);
    board[59] = ChessTile(ChessPiece(ChessPieceType::QUEEN, false), 3, 7);
    board[60] = ChessTile(ChessPiece(ChessPieceType::KING, false), 4, 7);
    board[61] = ChessTile(ChessPiece(ChessPieceType::BISHOP, false), 5, 7);
    board[62] = ChessTile(ChessPiece(ChessPieceType::KNIGHT, false), 6, 7);
    board[63] = ChessTile(ChessPiece(ChessPieceType::ROOK, false), 7, 7);
    // add initial board as first in the game history
    gameHistory.emplace_back(getStringFromBoard());
}

// ChessBoard::ChessBoard(const ChessBoard &otherBoard) {
//     for (const auto &otherTile : otherBoard.board) {
//         if (otherTile->piece == nullptr) {
//             board.push_back(std::make_unique<ChessTile>(nullptr, otherTile->getX(), otherTile->getY()));
//             continue;
//         }
//         board.push_back(std::make_unique<ChessTile>(std::make_unique<ChessPiece>(otherTile.getPiece().getType(),
//         otherTile.getPiece().isWhite()),
//                                                     otherTile->getX(), otherTile->getY()));
//     }
//     possibleMovesCache = otherBoard.possibleMovesCache;
//     whitesTurn = otherBoard.whitesTurn;
//     whiteRookMoved = otherBoard.whiteRookMoved;
//     blackRookMoved = otherBoard.blackRookMoved;
//     gameHistory = otherBoard.gameHistory;
//     markTurnForEnPassant = otherBoard.markTurnForEnPassant;
//     doublePawnMoveAt = otherBoard.doublePawnMoveAt;
//     enPassantPossibleLastMove = otherBoard.enPassantPossibleLastMove;
//     movesSinceLastCapture = otherBoard.movesSinceLastCapture;
//     doAfterMoveChecks = otherBoard.doAfterMoveChecks;
// }

// handles input usually by a user with checks for wrong inputs
GameState ChessBoard::handleInput(const std::string_view input) {
    const std::string color = whitesTurn ? "white " : "black ";
    if (input == "resign") {
        std::cout << color << "resigned" << std::endl;
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
    ChessTile &fromTile = moveTilePair->first;
    ChessTile &toTile = moveTilePair->second;

    if (!ChessMoveLogic::isInputMovePossible(fromTile, toTile)) return GameState::IN_PROGRESS;
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
    inputMove[1] = static_cast<char>(inputMove[1] - 1);
    inputMove[4] = static_cast<char>(inputMove[4] - 1);
    ChessTile &fromTile = getTileAt(inputMove.substr(0, 2));
    ChessTile &toTile = getTileAt(inputMove.substr(3));

    // make the move
    move(fromTile, toTile);

    // after move checks
    afterMoveChecks(toTile, pawnChangeTo);
}

// get a simple string of the current board
std::string ChessBoard::getStringFromBoard() {
    std::string outMoves;
    for (const auto &tile : board) {
        if (tile.getPiece().getType() == ChessPieceType::NONE) {
            outMoves += "_";
            continue;
        }
        if (tile.getPiece().isWhite()) {
            // white pieces in lower case
            outMoves += static_cast<char>(std::tolower(tile.getPiece().getShortName()));
            continue;
        }
        // black pieces in upper case
        outMoves += tile.getPiece().getShortName();
    }
    // TODO: thinking of writing a separate function to know if a en Passant is possible
    if (enPassantPossibleLastMove) {
        outMoves += "1";
    }
    return outMoves;
}

std::string ChessBoard::getMoveName(const ChessTile &fromTile, const ChessTile &toTile) {
    const std::string fromStr = ChessTile::mapIntToX.at(fromTile.getX()) + std::to_string(fromTile.getY() + 1);
    const std::string toStr = ChessTile::mapIntToX.at(toTile.getX()) + std::to_string(toTile.getY() + 1);
    return fromStr + ":" + toStr;
}

Pieces ChessBoard::getAllWhiteTiles() const {
    Pieces whiteTiles;
    for (auto &tile : board) {
        if (!hasPiece(tile)) continue;
        if (tile.getPiece().isWhite()) whiteTiles.push_back(&tile);
    }
    return whiteTiles;
}

Pieces ChessBoard::getAllBlackTiles() const {
    Pieces blackTiles;
    for (auto &tile : board) {
        if (!hasPiece(tile)) continue;
        if (!tile.getPiece().isWhite()) blackTiles.push_back(&tile);
    }
    return blackTiles;
}

Pieces ChessBoard::getWhitePieceType(ChessPieceType piece) const {
    Pieces whitePiecesType;
    for (auto &tile : board) {
        if (!hasPiece(tile)) continue;
        if (tile.getPiece().isWhite() && tile.getPiece().getType() == piece) {
            whitePiecesType.push_back(&tile);
        }
    }
    return whitePiecesType;
}

Pieces ChessBoard::getBlackPieceType(ChessPieceType piece) const {
    Pieces blackPiecesType;
    for (auto &tile : board) {
        if (!hasPiece(tile)) continue;
        if (!tile.getPiece().isWhite() && tile.getPiece().getType() == piece) {
            blackPiecesType.push_back(&tile);
        }
    }
    return blackPiecesType;
}

// TODO improve if I want to with better getAll.. but for single pieces not all
// of them
Pieces ChessBoard::getAllPossibleMovesPiece(const bool white, const ChessPieceType piece) const {
    Pieces allPossibleMoves;
    const Pieces colorPieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile : colorPieceTiles) {
        if (tile->getPiece().getType() == piece) {
            mergePossVec(allPossibleMoves, ChessMoveLogic::getPossibleMoves(*tile));
        }
    }
    return allPossibleMoves;
}

Pieces ChessBoard::getAllPiecesFor(const bool white, const ChessPieceType piece) const {
    Pieces pieces;
    const Pieces colorPieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile : colorPieceTiles) {
        if (tile->getPiece().getType() == piece) {
            pieces.push_back(tile);
        }
    }
    return pieces;
}

void ChessBoard::move(ChessTile &fromTile, ChessTile &toTile) {
    switch (fromTile.getPiece().getType()) {
        case ChessPieceType::PAWN:
            movePawn(fromTile, toTile);
            break;
        case ChessPieceType::KNIGHT:
            moveRook(fromTile);
            break;
        case ChessPieceType::KING:
            moveKing(fromTile, toTile);
        default:
            break;
    }

    // for optional or 100 moves draw rule
    if (hasPiece(toTile) || fromTile.getPiece().getType() == ChessPieceType::PAWN) {
        movesSinceLastCapture = 0;
    } else {
        movesSinceLastCapture++;
    }

    toTile.changePiece(std::move(fromTile.getPiece()));
    gameHistory.push_back(getStringFromBoard());
}

void ChessBoard::movePawn(const ChessTile &fromTile, const ChessTile &toTile) {
    // for that special pawn movement check if pawn moved more than two tiles.
    if (abs(fromTile.getY() - toTile.getY()) >= 2) {
        doublePawnMoveAt = std::make_pair(toTile.getX(), toTile.getY());
        markTurnForEnPassant = whitesTurn; // mark for delete next turn
    }
    // that special move happened so extra rule with capturing
    if (fromTile.getX() != toTile.getX() && toTile.getPiece().getType() == ChessPieceType::NONE) {
        const int whiteMove = whitesTurn ? -1 : 1;
        ChessTile &capturedPiece = getTileAt(toTile.getX(), toTile.getY() + whiteMove);
        capturedPiece.changePiece(ChessPiece{ChessPieceType::NONE, true});
    }
}

void ChessBoard::moveKing(const ChessTile &fromTile, const ChessTile &toTile) {
    // disables castling when the king is being moved
    if (fromTile.getPiece().isWhite() == true) {
        whiteRookMoved.first = true;
        whiteRookMoved.second = true;
    } else {
        blackRookMoved.first = true;
        blackRookMoved.second = true;
    }
    // do castling move for the rook
    if (abs(fromTile.getX() - toTile.getX()) >= 2) {
        const int x = toTile.getX();
        const int y = toTile.getY();
        if (x < 4) {
            ChessTile &rookTile = getTileAt(0, y);
            ChessTile &rookToTile = getTileAt(3, y);
            rookToTile.switchPiece(rookTile.getPiece());
        } else {
            ChessTile &rookTile = getTileAt(7, y);
            ChessTile &rookToTile = getTileAt(5, y);
            rookToTile.switchPiece(rookTile.getPiece());
        }
    }
}

inline void ChessBoard::moveRook(const ChessTile &fromTile) {
    const int x = fromTile.getX();
    const int y = fromTile.getY();
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

bool ChessBoard::validTilePos(std::string_view pos) const{
    if (pos.size() != 2) {
        std::cout << "ChessBoard::getTileAt: Wrong input length." << std::endl;
        return false;
    }
    const char xText = pos[0];
    if (xText != std::clamp(xText, 'a', 'h')) return false;
    const int x = ChessTile::mapXtoInt.at(xText);
    const int y = pos[1] - '0'; // is a trick to convert number char to int
    return validTilePos(x, y);
}

void ChessBoard::pawnWon(ChessTile &pawnTile, const char pawnToPiece) const {
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

GameState ChessBoard::afterMoveChecks(ChessTile &toTile, const char pawnToPiece) {
    if (markTurnForEnPassant != whitesTurn) {
        enPassantPossibleLastMove = false;
        doublePawnMoveAt = std::make_pair(-1, -1);
    }
    if (toTile.getPiece().getType() == ChessPieceType::PAWN && (toTile.getY() == 0 || toTile.getY() == 7)) {
        pawnWon(toTile, pawnToPiece);
    }
    // sometimes want to skip these for performance
    if (doAfterMoveChecks) {
        if (ChessMoveLogic::isDraw()) {
            std::cout << "this game is a draw!" << std::endl;
            return GameState::DRAW;
        }
        if (ChessMoveLogic::isKingCheckmate()) {
            std::cout << "you won the game!" << std::endl;
            return GameState::WON;
        }
    }
    whitesTurn = !whitesTurn;
    possibleMovesCache.clear();
    return GameState::IN_PROGRESS;
}

PiecePair ChessBoard::getMoveTilesFromInput(const std::string &input) {
    if (input.length() != 5) {
        std::cout << "ChessBoard::getMoveTilesFromInput: Wrong input length, should be 5." << std::endl;
        return std::nullopt;
    }
    std::string subStrFrom = input.substr(0, 2);
    std::string subStrTo = input.substr(3);
    subStrFrom[1] = static_cast<char>(subStrFrom[1] - 1);
    subStrTo[1] = static_cast<char>(subStrTo[1] - 1);
    if (!validTilePos(subStrFrom) || !validTilePos(subStrTo)) {
        std::cout << "ChessBoard::getMoveTilesFromInput: there is no tile like that" << std::endl;
        return std::nullopt;
    }
    ChessTile &fromTile = getTileAt(subStrFrom);
    ChessTile &toTile = getTileAt(subStrTo);
    if (fromTile.getPiece().getType() == ChessPieceType::NONE || fromTile.getPiece().isWhite() != whitesTurn) {
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
