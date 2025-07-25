#include "ChessMoveLogic.h"
#include "ChessBoard.h"
#include "ChessTile.h"

#include <algorithm>
#include <functional>
#include <utility>

ChessMoveLogic::ChessMoveLogic(const ChessBoard &board) : board(board) {}

Pieces ChessMoveLogic::getPossibleMoves(const ChessTile &fromTile) {
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesPawn, this, std::placeholders::_1));
        case ROOK:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesRook, this, std::placeholders::_1));
        case KNIGHT:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesKnight, this, std::placeholders::_1));
        case BISHOP:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesBishop, this, std::placeholders::_1));
        case QUEEN:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesQueen, this, std::placeholders::_1));
        case KING:
            // do insert and assign here because this will also include moves from castling which are not included sometimes
            return getPossibleMovesKing(fromTile);
            if (!possibleMovesCache.contains(&fromTile)) {
                possibleMovesCache[&fromTile] = getPossibleMovesKing(fromTile);
            }
            return possibleMovesCache.at(&fromTile);
        default:
            return {};
    }
}

// added because of recursion issues when checking castling moves
Pieces ChessMoveLogic::getPossibleMovesNoCastling(const ChessTile &fromTile) {
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesPawn, this, std::placeholders::_1));
        case ROOK:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesRook, this, std::placeholders::_1));
        case KNIGHT:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesKnight, this, std::placeholders::_1));
        case BISHOP:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesBishop, this, std::placeholders::_1));
        case QUEEN:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesQueen, this, std::placeholders::_1));
        case KING:
            return getPossibleMovesCached(fromTile, std::bind(&ChessMoveLogic::getPossibleMovesKingSingle, this, std::placeholders::_1));
        default:
            return {};
    }
}

Pieces ChessMoveLogic::getPossibleMovesUncached(const ChessTile &fromTile) {
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getPossibleMovesPawn(fromTile);
        case ROOK:
            return getPossibleMovesRook(fromTile);
        case KNIGHT:
            return getPossibleMovesKnight(fromTile);
        case BISHOP:
            return getPossibleMovesBishop(fromTile);
        case QUEEN:
            return getPossibleMovesQueen(fromTile);
        case KING:
            return getPossibleMovesKing(fromTile);
        default:
            return {};
    }
}

Pieces ChessMoveLogic::getPossibleMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func) {
    // const auto possMovesNoCache = func(fromTile);
    // return possMovesNoCache;
    if (!possibleMovesCache.contains(&fromTile)) {
        const auto possMoves = func(fromTile);
        possibleMovesCache.insert(std::make_pair(&fromTile, possMoves));
        return possMoves;
    }
    return possibleMovesCache.at(&fromTile);
}

Pieces ChessMoveLogic::getPossibleMovesPawn(const ChessTile &fromTile) {
    Pieces possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    const int white = fromTile.getPiece().isWhite() ? 1 : -1;
    // some different rules for pawn applying again
    if (ChessBoard::validTilePos(x, y + white) && addIfPossibleMove(fromTile, board.getTileAt(x, y + white), possibleMoves)) {
        // pawn on starting position can move two tiles
        if (white == 1 && y == 1) {
            addIfPossibleMove(fromTile, board.getTileAt(x, y + 2 * white), possibleMoves);
        }
        if (white == -1 && y == 6) {
            addIfPossibleMove(fromTile, board.getTileAt(x, y + 2 * white), possibleMoves);
        }
    }
    // capture other pawn
    if (ChessBoard::validTilePos(x + 1, y + white)) addIfPossibleMove(fromTile, board.getTileAt(x + 1, y + white), possibleMoves);
    if (ChessBoard::validTilePos(x - 1, y + white)) addIfPossibleMove(fromTile, board.getTileAt(x - 1, y + white), possibleMoves);
    // get pawn move for that special pawn move that never happens
    if (board.getLastDoublePawnMove().second == y) {
        if (x - 1 == board.getLastDoublePawnMove().first) {
            // TODO: wanted to know if in the current turn a en passant is possible. Better move
            // somewhere else enPassantPossible = true;
            if (ChessBoard::validTilePos(x - 1, y + white)) possibleMoves.push_back(&board.getTileAt(x - 1, y + white));
        } else if (x + 1 == board.getLastDoublePawnMove().first) {
            if (ChessBoard::validTilePos(x + 1, y + white)) possibleMoves.push_back(&board.getTileAt(x + 1, y + white));
        }
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesBishop(const ChessTile &fromTile) {
    static const std::vector directions = {std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirection(fromTile, directions);
}

Pieces ChessMoveLogic::getPossibleMovesKnight(const ChessTile &fromTile) {
    static const std::vector directions = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                           std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    return getPossibleMovesByDirectionSingle(fromTile, directions);
}

Pieces ChessMoveLogic::getPossibleMovesRook(const ChessTile &fromTile) {
    static const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    return getPossibleMovesByDirection(fromTile, directions);
}

Pieces ChessMoveLogic::getPossibleMovesQueen(const ChessTile &fromTile) {
    static const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                           std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirection(fromTile, directions);
}

Pieces ChessMoveLogic::getPossibleMovesKing(const ChessTile &fromTile) {
    Pieces possibleMoves = getPossibleMovesKingSingle(fromTile);
    ChessBoard::mergePossVec(possibleMoves, getPossibleMovesCastling(fromTile));
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesKingSingle(const ChessTile &fromTile) {
    static const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                           std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirectionSingle(fromTile, directions);
}

Pieces ChessMoveLogic::getPossibleMovesCastling(const ChessTile &fromTile) {
    Pieces possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    if (board.isWhitesTurn()) {
        const auto &[whiteRookLeft, whiteRookRight] = board.getWhiteRookMoved();
        if (!whiteRookLeft) {
            const Pieces tilesToCheckLeft = getLeftCastleTiles(x, y);
            if (!isTileAttacked(false, tilesToCheckLeft) && isTileFree(tilesToCheckLeft)) {
                possibleMoves.push_back(&board.getTileAt(x - 2, y));
            }
        }
        if (!whiteRookRight) {
            const Pieces tilesToCheckRight = getRightCastleTiles(x, y);
            if (!isTileAttacked(false, tilesToCheckRight) && isTileFree(tilesToCheckRight)) {
                possibleMoves.push_back(&board.getTileAt(x + 2, y));
            }
        }
    } else {
        const auto &[blackRookLeft, blackRookRight] = board.getBlackRookMoved();
        if (!blackRookLeft) {
            const Pieces tilesToCheckLeft = getLeftCastleTiles(x, y);
            if (!isTileAttacked(true, tilesToCheckLeft) && isTileFree(tilesToCheckLeft)) {
                possibleMoves.push_back(&board.getTileAt(x - 2, y));
            }
        }
        if (!blackRookRight) {
            const Pieces tilesToCheckRight = getRightCastleTiles(x, y);
            if (!isTileAttacked(true, tilesToCheckRight) && isTileFree(tilesToCheckRight)) {
                possibleMoves.push_back(&board.getTileAt(x + 2, y));
            }
        }
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesByDirection(const ChessTile &fromTile, const std::vector<std::pair<int, int>> &directions) const {
    Pieces possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    for (const auto &[xDirection, yDirection] : directions) {
        for (int i = 1; i < 8; i++) { // TODO: check loop range so tile valid does not have to be checked
            const int nextX = x + i * xDirection;
            const int nextY = y + i * yDirection;

            if (!ChessBoard::validTilePos(nextX, nextY)) break;
            const ChessTile &nextTile = board.getTileAt(nextX, nextY);
            // either it reaches the end or movement gets blocked
            if (!addIfPossibleMove(fromTile, nextTile, possibleMoves)) break;
        }
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesByDirectionSingle(const ChessTile &fromTile,
                                                         const std::vector<std::pair<int, int>> &directions) const {
    Pieces possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    for (const auto &[xDirection, yDirection] : directions) {
        const int nextX = x + xDirection;
        const int nextY = y + yDirection;

        if (!ChessBoard::validTilePos(nextX, nextY)) continue;
        const ChessTile &nextTile = board.getTileAt(nextX, nextY);
        addIfPossibleMove(fromTile, nextTile, possibleMoves);
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getAllPossibleMoves(const bool white) {
    Pieces allPossibleMoves;
    const Pieces pieceTiles = white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        Pieces newPossibleMoves = getPossibleMovesNoCastling(*tile);
        filterPossibleMovesForChecks(*tile, newPossibleMoves);
        ChessBoard::mergePossVec(allPossibleMoves, newPossibleMoves);
    }
    return allPossibleMoves;
}

// TODO improve if I want to with better getAll.. but for single pieces not all
// of them
Pieces ChessMoveLogic::getAllPossibleMovesPiece(const bool white, const ChessPieceType piece) {
    Pieces allPossibleMoves;
    const Pieces colorPieceTiles = white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : colorPieceTiles) {
        if (tile->hasPiece(piece)) {
            ChessBoard::mergePossVec(allPossibleMoves, getPossibleMoves(*tile));
        }
    }
    return allPossibleMoves;
}

bool ChessMoveLogic::isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile) {
    // get the possible moves for the from tile
    Pieces possibleMoves = getPossibleMoves(fromTile);
    filterPossibleMovesForChecks(fromTile, possibleMoves);
    // check if to tile is in those possible moves
    const auto itPossMove = std::ranges::find(possibleMoves, &toTile);
    return itPossMove != possibleMoves.end();
}

void ChessMoveLogic::filterPossibleMovesForChecks(const ChessTile &fromTile, Pieces &possibleMoves) {
    std::erase_if(possibleMoves, [this, &fromTile](const ChessTile *toTile) {
        // can not castle with checked king
        if (fromTile.hasPiece(KING) && std::abs(fromTile.getX() - toTile->getX()) == 2) {
            return isKingChecked(board.isWhitesTurn());
        }
        return isKingCheckedAfterMove(fromTile, *toTile);
    });
}

// returns true if it is a free tile
bool ChessMoveLogic::addIfPossibleMove(const ChessTile &fromTile, const ChessTile &toTile, Pieces &possibleMoves) {
    if (!toTile.hasPiece(NONE)) {                                          // there is a piece
        if (fromTile.hasPiece(PAWN) && fromTile.getX() == toTile.getX()) { // pawn only piece that can't capture piece directly in its path
            return false;
        }
        // piece are a different color so add it
        if (fromTile.hasWhitePiece() != toTile.hasWhitePiece()) {
            possibleMoves.push_back(&toTile);
        }
        return false; // piece is same color so don't add it
    }
    if (fromTile.hasPiece(PAWN) && fromTile.getX() != toTile.getX() && toTile.hasPiece(NONE)) // for that pawn move
        return false;
    possibleMoves.push_back(&toTile); // tile is free to move on
    return true;
}

// check if tile attacked by @param white that color
bool ChessMoveLogic::isTileAttacked(const bool white, const Pieces &tilesToCheck) {
    const Pieces pieceTiles = white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        const Pieces possMoves = getPossibleMovesNoCastling(*tile);
        for (const ChessTile *tileToCheck : tilesToCheck) {
            if (std::ranges::find(possMoves, tileToCheck) != possMoves.end()) {
                return true;
            }
        }
    }
    return false;
}

///
/// @param _board the chess _board to check this
/// @param white The color to check for being checked
/// @param cached if the cached positions should be checked
/// @return if color is checked
bool ChessMoveLogic::isKingChecked(const bool white) {
    const Pieces pieceTiles = !white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        // if (tile->hasPiece(KING)) continue;

        const Pieces possMoves = getPossibleMovesNoCastling(*tile);
        const bool found = std::ranges::any_of(possMoves, [white](const ChessTile *possMove) {
            return possMove->hasPiece(KING) && possMove->hasWhitePiece() == white; 
        });
        if (found) {
            return true;
        }
    }
    return false;
}

/// makes a move and check if King is checked after
/// @param fromTile the tile to move from
/// @param toTile the tile to move to
/// @return if King is checked after that move
bool ChessMoveLogic::isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile) {
    // create new board to simulate the move
    ChessBoard checkBoard(board);
    ChessMoveLogic checkMoveLogic(checkBoard);
    ChessTile &checkFromTile = checkBoard.getTileAt(fromTile.getX(), fromTile.getY());
    ChessTile &checkToTile = checkBoard.getTileAt(toTile.getX(), toTile.getY());
    checkBoard.move(checkFromTile, checkToTile);

    return checkMoveLogic.isKingChecked(!checkBoard.isWhitesTurn());
}

bool ChessMoveLogic::isKingCheckmate(const bool white) {
    const Pieces pieceTiles = white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        Pieces possMoves = getPossibleMovesNoCastling(*tile);
        filterPossibleMovesForChecks(*tile, possMoves);
        if (!possMoves.empty()) {
            return false;
        }
    }
    return true;
}

bool ChessMoveLogic::isDraw(const bool white) {
    // if (isThreefoldRepetition()) // seems optional so deactivate for now
    // return true; // self explaining :)
    if (getAllPossibleMoves(white).size() <= 0 && !isKingChecked(white)) {
        return true; // stalemate
    }
    // if (movesSinceLastCapture > 100) // maybe that is optional too but leave like this for now
    // return true; // 100 half turns with no capture or pawn move
    // check for any other dead position where a win is not possible anymore
    if (isDeadPosition()) {
        return true;
    }
    return false;
}

bool ChessMoveLogic::isThreefoldRepetition() const {
    auto countBoardStr = [this](const std::string &_boardStr) { return std::ranges::count(board.getGameHistory(), _boardStr) >= 3; };
    return std::ranges::any_of(board.getGameHistory(), countBoardStr);
}

bool ChessMoveLogic::isDeadPosition() const {
    const Pieces whitePieces = board.getAllWhiteTiles();
    const Pieces blackPieces = board.getAllBlackTiles();
    const size_t whiteSize = whitePieces.size();
    const size_t blackSize = blackPieces.size();
    if (whiteSize > 2 || blackSize > 2) return false;  // always still possible if more then two pieces
    if (whiteSize == 1 && blackSize == 1) return true; // only kings left

    auto is_Bishop = [](const ChessTile *tile) { return tile->hasPiece(BISHOP); };

    auto is_Knight = [](const ChessTile *tile) { return tile->hasPiece(KNIGHT); };

    const auto itWhiteBishop = std::ranges::find_if(whitePieces, is_Bishop);
    const auto itBlackBishop = std::ranges::find_if(blackPieces, is_Bishop);
    const auto itWhiteKnight = std::ranges::find_if(whitePieces, is_Knight);
    const auto itBlackKnight = std::ranges::find_if(blackPieces, is_Knight);
    if (whitePieces.size() == 2 && blackPieces.size() == 2) {
        // only if both are bishop and they are on the same color tile
        if (itWhiteBishop == whitePieces.end()) return false;
        if (itBlackBishop == blackPieces.end()) return false;
        if ((*itWhiteBishop)->hasWhitePiece() == (*itBlackBishop)->hasWhitePiece()) return true;
        return false;
    }
    // if the only other piece in the game is one of them also a dead position
    // draw
    if (itWhiteBishop != whitePieces.end() || itWhiteKnight != whitePieces.end() || itBlackBishop != blackPieces.end() ||
        itBlackKnight != blackPieces.end())
        return true;
    return false;
}
