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
    system("clear");
    // Print the chessboard with characters
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((i + j) % 2 == 0) {
                // White square
                std::cout << "\x1b[48;5;231m";
            } else {
                // Black square
                std::cout << "\x1b[48;5;0m";
            }
            if (getTileAt(j, i)->piece == nullptr) {
                std::cout << "   ";
                continue;
            }
            if (getTileAt(j, i)->piece->isWhite()) {
                // green
                std::cout << "\x1b[38;5;22m";
            } else {
                // magenta
                std::cout << "\x1b[38;5;126m";
            }
            std::cout << " " << getTileAt(j, i)->piece->getShortName() << " ";
        }
        std::cout << "\x1b[0m" << std::endl;  // Reset to default color after each line
    }
}

std::vector<ChessTile *> ChessBoard::getPossibleMoves(const ChessTile *fromTile) const {
    std::vector<ChessTile *> possibleMoves;
    if (fromTile->piece == nullptr) return possibleMoves;
    const ChessPiece *piece = fromTile->piece;
    switch(piece->getType()) {
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

std::vector<ChessTile *> ChessBoard::getPossibleMovesPawn(const ChessTile *fromTile) const {
    std::vector<ChessTile *> possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const int white = fromTile->piece->isWhite() ? 1 : -1;
    if (getTileAt(x, y + white)->piece == nullptr) {
        possibleMoves.push_back(getTileAt(x, y + white));
        if (white && y == 1 && getTileAt(x, y + 2 * white)->piece == nullptr) {
            possibleMoves.push_back(getTileAt(x, y + 2 * white));
        }
    } else {
        possibleMoves.push_back(getTileAt(x, y + white));
        if (white && y == 6 && getTileAt(x, y + 2 * white)->piece == nullptr) {
            possibleMoves.push_back(getTileAt(x, y + 2 * white));
        }
    }
    if (!getTileAt(x - 1, y + white)->piece->isWhite()) {
        possibleMoves.push_back(getTileAt(x - 1, y + white));
    }
    if (!getTileAt(x + 1, y + white)->piece->isWhite()) {
        possibleMoves.push_back(getTileAt(x + 1, y + white));
    }
    // get pawn move for that special pawn move that never happens
    if (doublePawnMoveAt != -1) {
        if (x - 1 == doublePawnMoveAt) {
            possibleMoves.push_back(getTileAt(x - 1, y + white));
        } else if (x + 1 == doublePawnMoveAt) {
            possibleMoves.push_back(getTileAt(x + 1, y + white));
        }
    }
    return possibleMoves;
}

std::vector<ChessTile *> ChessBoard::getPossibleMovesBishop(const ChessTile *fromTile) const {
    const std::vector directions = {std::pair(1, 1), std::pair(1, -1),
                                    std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirection(fromTile, directions);
}

std::vector<ChessTile *> ChessBoard::getPossibleMovesRook(const ChessTile *fromTile) const {
    const std::vector directions = {std::pair(0, 1), std::pair(0, -1),
                                    std::pair(-1, 0), std::pair(1, 0)};
    return getPossibleMovesByDirection(fromTile, directions);
}

std::vector<ChessTile *> ChessBoard::getPossibleMovesQueen(const ChessTile *fromTile) const {
    std::vector<ChessTile *> possibleMoves = getPossibleMovesBishop(fromTile);
    mergePossVec(possibleMoves, getPossibleMovesRook(fromTile));
    return possibleMoves;
}

std::vector<ChessTile *> ChessBoard::getPossibleMovesKnight(const ChessTile *fromTile) const {
    const std::vector directions = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                    std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    return getPossibleMovesByDirectionSingle(fromTile, directions);
}

std::vector<ChessTile *> ChessBoard::getPossibleMovesKing(const ChessTile *fromTile) const {
    std::vector<ChessTile *> possibleMoves;
    const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1), std::pair(1, -1),
                                    std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    mergePossVec(possibleMoves, getPossibleMovesByDirectionSingle(fromTile, directions));
    return possibleMoves;
    //TODO: castling
}

std::vector<ChessTile *> ChessBoard::getPossibleMovesByDirection(const ChessTile *fromTile,
                                                                 const std::vector<std::pair<int, int>> &directions) const {
    std::vector<ChessTile *> possibleMoves;
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

std::vector<ChessTile *> ChessBoard::getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                                                       const std::vector<std::pair<int, int>> &directions) const {
    std::vector<ChessTile *> possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (std::pair pair : directions) {
        const int xDirection = pair.first;
        const int yDirection = pair.second;
        ChessTile *nextTile = getTileAt(x + xDirection, y + yDirection);
        if (!isPossibleMove(fromTile->piece->isWhite(), nextTile, possibleMoves)) break;
    }
    return possibleMoves;
}

bool ChessBoard::isPossibleMove(const bool fromTileWhite, ChessTile *toTile,
                                std::vector<ChessTile *> &possibleMoves) const {
    if (toTile == nullptr) return false;
    if (fromTileWhite == toTile->piece->isWhite()) return false;
    possibleMoves.push_back(toTile);
    return true;
}

void ChessBoard::handleMoveInput(const std::string &input) const {
    if (input.length() != 5) {
        std::cerr << "ChessBoard::move: Wrong input length, should be 5." << std::endl;
    }
    ChessTile *fromTile = getTileAt(input.substr(0,2));
    ChessTile *toTile = getTileAt(input.substr(3,2));
    move(fromTile, toTile);
}

void ChessBoard::move(ChessTile *fromTile, ChessTile *toTile) const {
    toTile->piece = fromTile->piece;
    fromTile->piece = nullptr;
}

ChessTile *ChessBoard::getTileAt(const std::string &pos) const {
    if (pos.size() != 2) {
        std::cerr << "ChessBoard::getTileAt: Wrong input length." << std::endl;
        return nullptr;
    }
    const char xText = pos[0];
    if (xText >= 'a' && xText <= 'h') return nullptr;
    const int x = mapXtoInt.at(xText);
    const int y = pos[1];
    return getTileAt(x, y);
}

ChessTile *ChessBoard::getTileAt(const int x, const int y) const {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) return nullptr;
    return board[y * boardWidth + x];
}

void ChessBoard::mergePossVec(std::vector<ChessTile *> &possibleMoves,
                              std::vector<ChessTile *> possibleMovesMerge) const {
    possibleMoves.insert(possibleMoves.end(), possibleMovesMerge.begin(), possibleMovesMerge.end());
}
