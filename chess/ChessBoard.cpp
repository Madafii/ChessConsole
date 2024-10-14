//
// Created by fpittermann on 10/5/24.
//

#include "ChessBoard.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>

ChessBoard::ChessBoard() {
    mapXtoInt['a'] = 0;
    mapXtoInt['b'] = 1;
    mapXtoInt['c'] = 2;
    mapXtoInt['d'] = 3;
    mapXtoInt['e'] = 4;
    mapXtoInt['f'] = 5;
    mapXtoInt['g'] = 6;
    mapXtoInt['h'] = 7;

    initBoard();
}

void ChessBoard::initBoard() {
    board.push_back(new ChessTile(new ChessPiece(Rook, true), 0, 0));
    board.push_back(new ChessTile(new ChessPiece(Knight, true), 1, 0));
    board.push_back(new ChessTile(new ChessPiece(Bishop, true), 2, 0));
    board.push_back(new ChessTile(new ChessPiece(Queen, true), 3, 0));
    board.push_back(new ChessTile(new ChessPiece(King, true), 4, 0));
    board.push_back(new ChessTile(new ChessPiece(Bishop, true), 5, 0));
    board.push_back(new ChessTile(new ChessPiece(Knight, true), 6, 0));
    board.push_back(new ChessTile(new ChessPiece(Rook, true), 7, 0));
    for (int i = 0; i < boardWidth; i++) {
        board.push_back(new ChessTile(new ChessPiece(Pawn, true), i, 1));
    }
    for (int i = 0; i < boardWidth * 4; i++) {
        board.push_back(new ChessTile(nullptr, i % boardWidth, i / boardWidth + 2));
    }
    for (int i = 0; i < boardWidth; i++) {
        board.push_back(new ChessTile(new ChessPiece(Pawn, false), i, 6));
    }
    board.push_back(new ChessTile(new ChessPiece(Rook, false), 0, 7));
    board.push_back(new ChessTile(new ChessPiece(Knight, false), 1, 7));
    board.push_back(new ChessTile(new ChessPiece(Bishop, false), 2, 7));
    board.push_back(new ChessTile(new ChessPiece(Queen, false), 3, 7));
    board.push_back(new ChessTile(new ChessPiece(King, false), 4, 7));
    board.push_back(new ChessTile(new ChessPiece(Bishop, false), 5, 7));
    board.push_back(new ChessTile(new ChessPiece(Knight, false), 6, 7));
    board.push_back(new ChessTile(new ChessPiece(Rook, false), 7, 7));
}

void ChessBoard::updateBoard() const {
    // system("clear"); // disable for debugging
    // Print the chessboard with characters
    for (int y = 7; y >= 0; --y) {
        std::cout << " " << y + 1 << " ";
        for (int x = 0; x < 8; ++x) {
            if ((y + x) % 2 == 0) {
                // Black square
                std::cout << "\x1b[48;5;0m";
            } else {
                // White square
                std::cout << "\x1b[48;5;231m";
            }
            if (getTileAt(x, y)->piece == nullptr) {
                std::cout << "   ";
                continue;
            }
            if (getTileAt(x, y)->piece->isWhite()) {
                // green
                std::cout << "\x1b[38;5;22m";
            } else {
                // magenta
                std::cout << "\x1b[38;5;126m";
            }
            std::cout << " " << getTileAt(x, y)->piece->getShortName() << " ";
        }
        std::cout << "\x1b[0m" << std::endl;  // Reset to default color after each line
    }
    std::cout << "    a  b  c  d  e  f  g  h " << std::endl;
}

Pieces ChessBoard::getPossibleMoves(const ChessTile *fromTile) {
    Pieces possibleMoves;
    if (fromTile->piece == nullptr) return possibleMoves;
    // const ChessPiece *piece = fromTile->piece;
    switch(fromTile->piece->getType()) {
        case Pawn:
            mergePossVec(possibleMoves, getPossibleMovesPawn(fromTile));
        break;
        case Rook:
            mergePossVec(possibleMoves, getPossibleMovesRook(fromTile));
        break;
        case Knight:
            mergePossVec(possibleMoves, getPossibleMovesKnight(fromTile));
        break;
        case Bishop:
            mergePossVec(possibleMoves, getPossibleMovesBishop(fromTile));
        break;
        case Queen:
            mergePossVec(possibleMoves, getPossibleMovesQueen(fromTile));
        break;
        case King:
            mergePossVec(possibleMoves, getPossibleMovesKing(fromTile));
        break;
        default:
            return possibleMoves;
    }
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesPawn(const ChessTile *fromTile) const {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const int white = fromTile->piece->isWhite() ? 1 : -1;
    if (isPossibleMove(fromTile->piece->isWhite(), getTileAt(x, y + white), possibleMoves)) {
        if (white == 1 && y == 1 && getTileAt(x, y + 2 * white)->piece == nullptr) {
            possibleMoves.push_back(getTileAt(x, y + 2 * white));
        }
        if (white == -1 && y == 6 && getTileAt(x, y + 2 * white)->piece == nullptr) {
            possibleMoves.push_back(getTileAt(x, y + 2 * white));
        }
    }
    isPossibleMove(fromTile->piece->isWhite(), getTileAt(x + 1, y + white), possibleMoves, true);
    isPossibleMove(fromTile->piece->isWhite(), getTileAt(x - 1, y + white), possibleMoves, true);
    // get pawn move for that special pawn move that never happens
    if (doublePawnMoveAt != -1) {
        // TODO: add weird pawn move
        if (x - 1 == doublePawnMoveAt) {
            possibleMoves.push_back(getTileAt(x - 1, y + white));
        } else if (x + 1 == doublePawnMoveAt) {
            possibleMoves.push_back(getTileAt(x + 1, y + white));
        }
    }
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesBishop(const ChessTile *fromTile) const {
    const std::vector directions = {std::pair(1, 1), std::pair(1, -1),
                                    std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirection(fromTile, directions);
}

Pieces ChessBoard::getPossibleMovesRook(const ChessTile *fromTile) const {
    const std::vector directions = {std::pair(0, 1), std::pair(0, -1),
                                    std::pair(-1, 0), std::pair(1, 0)};
    return getPossibleMovesByDirection(fromTile, directions);
}

Pieces ChessBoard::getPossibleMovesQueen(const ChessTile *fromTile) const {
    Pieces possibleMoves = getPossibleMovesBishop(fromTile);
    mergePossVec(possibleMoves, getPossibleMovesRook(fromTile));
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesKnight(const ChessTile *fromTile) const {
    const std::vector directions = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                    std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    return getPossibleMovesByDirectionSingle(fromTile, directions);
}

Pieces ChessBoard::getPossibleMovesKing(const ChessTile *fromTile) const {
    Pieces possibleMoves;
    const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1), std::pair(1, -1),
                                    std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    mergePossVec(possibleMoves, getPossibleMovesByDirectionSingle(fromTile, directions));
    return possibleMoves;
    //TODO: castling
}

Pieces ChessBoard::getPossibleMovesByDirection(const ChessTile *fromTile,
                                                                 const std::vector<std::pair<int, int>> &directions) const {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (std::pair pair : directions) {
        const int xDirection = pair.first;
        const int yDirection = pair.second;
        for (int i = 0; i < 8; i++) {
            ChessTile *nextTile = getTileAt(x + i * xDirection, y + i * yDirection);
            if (!isPossibleMove(fromTile->piece->isWhite(), nextTile, possibleMoves)) break;
        }
    }
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                                                       const std::vector<std::pair<int, int>> &directions) const {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (std::pair pair : directions) {
        const int xDirection = pair.first;
        const int yDirection = pair.second;
        ChessTile *nextTile = getTileAt(x + xDirection, y + yDirection);
        isPossibleMove(fromTile->piece->isWhite(), nextTile, possibleMoves);
    }
    return possibleMoves;
}

Pieces ChessBoard::getAllWhiteTiles() const {
    Pieces whiteTiles;
    for (ChessTile* tile : board) {
        if (tile->piece == nullptr) continue;
        if (tile->piece->isWhite()) whiteTiles.push_back(tile);
    }
    return whiteTiles;
}

Pieces ChessBoard::getAllBlackTiles() const {
    Pieces blackTiles;
    for (ChessTile* tile : board) {
        if (tile->piece == nullptr) continue;
        if (!tile->piece->isWhite()) blackTiles.push_back(tile);
    }
    return blackTiles;
}

void ChessBoard::filterPossibleMovesForChecks(const ChessTile *fromTile, Pieces &possibleMoves) {
    // check if the move would make the player checked which is an illegal move
    possibleMoves.erase(std::remove_if(possibleMoves.begin(), possibleMoves.end(), [fromTile, this](ChessTile* toTile) {
        ChessPiece *tmpPieceFrom = fromTile->piece;
        ChessPiece *tmpPieceTo = toTile->piece;
        toTile->piece = tmpPieceFrom;
        const bool isChecked = isKingChecked(!whitesTurn);
        toTile->piece = tmpPieceTo;
        return isChecked;
    }), possibleMoves.end());
}

bool ChessBoard::isPossibleMove(const bool fromTileWhite, ChessTile *toTile,
                                Pieces &possibleMoves, const bool isPawnSpecialMove) const {
    if (toTile == nullptr) return false;
    if (toTile->piece != nullptr && fromTileWhite == toTile->piece->isWhite()) return false;
    if (isPawnSpecialMove && toTile->piece == nullptr) return false;
    possibleMoves.push_back(toTile);
    return true;
}

// if king of bool white is checked in the current possition
bool ChessBoard::isKingChecked(const bool white) {
    const Pieces pieceTiles = !white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile* tile : pieceTiles) {
        for (const ChessTile* possMove : getPossibleMoves(tile)) {
            if (possMove->piece == nullptr) continue;
            if (possMove->piece->getType() == King) {
                return true;
            }
        }
    }
    return false;
}

bool ChessBoard::isKingCheckmate() {

}

void ChessBoard::handleMoveInput(const std::string &input) {
    if (input.length() != 5) {
        std::cerr << "ChessBoard::handleMoveInput: Wrong input length, should be 5." << std::endl;
        return;
    }
    std::string subStrFrom = input.substr(0, 2);
    std::string subStrTo = input.substr(3);
    subStrFrom[1] = subStrFrom[1] - 1;
    subStrTo[1] = subStrTo[1] - 1;
    ChessTile *fromTile = getTileAt(subStrFrom);
    ChessTile *toTile = getTileAt(subStrTo);
    if (fromTile == nullptr || toTile == nullptr) {
        std::cerr << "ChessBoard::handleMoveInput: there is no tile like that" << std::endl;
        return;
    }
    if (fromTile->piece == nullptr || fromTile->piece->isWhite() != whitesTurn) {
        std::cerr << "ChessBoard::handleMoveInput: trying to move a piece from the opponent or no piece" << std::endl;
        return;
    }
    Pieces possibleMoves = getPossibleMoves(fromTile);
    filterPossibleMovesForChecks(fromTile, possibleMoves);
    std::cout << "the possible moves are:" << std::endl;
    for (const auto possMove : possibleMoves) {
        std::cout << possMove->getX() << " " << possMove->getY() << std::endl;
    }
    const auto itPossMove = std::find(possibleMoves.begin(), possibleMoves.end(), toTile);
    if (itPossMove == possibleMoves.end()) {
        std::cerr << "ChessBoard::handleMoveInput: that is not a possible move" << std::endl;
        return;
    }
    move(fromTile, toTile);
    whitesTurn = !whitesTurn;
}

void ChessBoard::move(ChessTile *fromTile, ChessTile *toTile) {
    toTile->piece = fromTile->piece;
    fromTile->piece = nullptr;
}

ChessTile *ChessBoard::getTileAt(const std::string &pos) const {
    if (pos.size() != 2) {
        std::cerr << "ChessBoard::getTileAt: Wrong input length." << std::endl;
        return nullptr;
    }
    const char xText = pos[0];
    if (xText != std::clamp(xText, 'a', 'h')) return nullptr;
    const int x = mapXtoInt.at(xText);
    const int y = pos[1] - '0'; // is a trick to convert number char to int
    return getTileAt(x, y);
}

ChessTile *ChessBoard::getTileAt(const int x, const int y) const {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) return nullptr;
    return board[y * boardWidth + x];
}

void ChessBoard::mergePossVec(Pieces &possibleMoves,
                              Pieces possibleMovesMerge) const {
    possibleMoves.insert(possibleMoves.end(), possibleMovesMerge.begin(), possibleMovesMerge.end());
}
