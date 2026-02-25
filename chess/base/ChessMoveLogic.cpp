#include "ChessMoveLogic.h"
#include "ChessBoard.h"
#include "ChessTile.h"

#include <algorithm>
#include <functional>
#include <utility>

ChessMoveLogic::ChessMoveLogic(const ChessBoard &board) : _board(board) {}

// return legal moves
PieceTiles ChessMoveLogic::getLegalMoves(const ChessTile &fromTile) const {
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getMovesCached(fromTile, std::bind(&ChessMoveLogic::getMovesPawn, this, std::placeholders::_1));
        case ROOK:
            return getMovesCached(fromTile, std::bind(&ChessMoveLogic::getMovesRook, this, std::placeholders::_1));
        case KNIGHT:
            return getMovesCached(fromTile, std::bind(&ChessMoveLogic::getMovesKnight, this, std::placeholders::_1));
        case BISHOP:
            return getMovesCached(fromTile, std::bind(&ChessMoveLogic::getMovesBishop, this, std::placeholders::_1));
        case QUEEN:
            return getMovesCached(fromTile, std::bind(&ChessMoveLogic::getMovesQueen, this, std::placeholders::_1));
        case KING:
            return getMovesCached(fromTile, std::bind(&ChessMoveLogic::getMovesKing, this, std::placeholders::_1));
        default:
            return {};
    }
}

// return possible moves for fromTile without checking for legality, meaning if the king is checked
PieceTiles ChessMoveLogic::getMoves(const ChessTile &fromTile) const {
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getMovesPawn(fromTile);
        case ROOK:
            return getMovesRook(fromTile);
        case KNIGHT:
            return getMovesKnight(fromTile);
        case BISHOP:
            return getMovesBishop(fromTile);
        case QUEEN:
            return getMovesQueen(fromTile);
        case KING:
            return getMovesKing(fromTile);
        default:
            return {};
    }
}

// return possible moves without caching as that will check for the legality of a move, which will simulate the next move, which results in
// a recursive call
PieceTiles ChessMoveLogic::getMovesNoCastling(const ChessTile &fromTile) const {
    switch (fromTile.getPieceType()) {
        case PAWN:
            return getMovesPawn(fromTile);
        case ROOK:
            return getMovesRook(fromTile);
        case KNIGHT:
            return getMovesKnight(fromTile);
        case BISHOP:
            return getMovesBishop(fromTile);
        case QUEEN:
            return getMovesQueen(fromTile);
        case KING:
            return getMovesKingSingle(fromTile);
        default:
            return {};
    }
}

// TODO: rework caching
PieceTiles ChessMoveLogic::getMovesCached(const ChessTile &fromTile, const possibleMovesFunc &func) const {
    // if (!_possibleMovesCache.contains(&fromTile)) {
        PieceTiles possMoves = func(fromTile);
        filterLegalMoves(fromTile, possMoves);
        // _possibleMovesCache.emplace(&fromTile, possMoves);
        return possMoves;
    // }
    // return _possibleMovesCache.at(&fromTile);
}

PieceTiles ChessMoveLogic::getMovesPawn(const ChessTile &fromTile) const {
    PieceTiles possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    const int white = fromTile.getPiece().isWhite() ? 1 : -1;
    // double move on starter position
    if (ChessBoard::validTilePos(x, y + white) && addIfLegalMove(fromTile, _board.getTileAt(x, y + white), possibleMoves)) {
        if (white == 1 && y == 1) {
            addIfLegalMove(fromTile, _board.getTileAt(x, y + 2 * white), possibleMoves);
        }
        if (white == -1 && y == 6) {
            addIfLegalMove(fromTile, _board.getTileAt(x, y + 2 * white), possibleMoves);
        }
    }
    // capture other pawn
    if (ChessBoard::validTilePos(x + 1, y + white)) addIfLegalMove(fromTile, _board.getTileAt(x + 1, y + white), possibleMoves);
    if (ChessBoard::validTilePos(x - 1, y + white)) addIfLegalMove(fromTile, _board.getTileAt(x - 1, y + white), possibleMoves);
    // get pawn move for that special pawn move that never happens
    if (_board.getLastDoublePawnMove().second == y) {
        if (x - 1 == _board.getLastDoublePawnMove().first) {
            if (ChessBoard::validTilePos(x - 1, y + white)) possibleMoves.push_back(&_board.getTileAt(x - 1, y + white));
        } else if (x + 1 == _board.getLastDoublePawnMove().first) {
            if (ChessBoard::validTilePos(x + 1, y + white)) possibleMoves.push_back(&_board.getTileAt(x + 1, y + white));
        }
    }
    return possibleMoves;
}

PieceTiles ChessMoveLogic::getMovesKing(const ChessTile &fromTile) const {
    PieceTiles possibleMoves = getMovesKingSingle(fromTile);
    ChessBoard::mergePossVec(possibleMoves, getMovesCastling(fromTile));
    return possibleMoves;
}

PieceTiles ChessMoveLogic::getMovesCastling(const ChessTile &fromTile) const {
    PieceTiles possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    if (_board.isWhitesTurn()) {
        const auto &[whiteRookLeft, whiteRookRight] = _board.getWhiteRookMoved();
        if (!whiteRookLeft) {
            const PieceTiles tilesToCheckLeft = getLeftCastleTiles(x, y);
            if (!isTileAttacked(false, tilesToCheckLeft) && isTileFree(tilesToCheckLeft)) {
                possibleMoves.push_back(&_board.getTileAt(x - 2, y));
            }
        }
        if (!whiteRookRight) {
            const PieceTiles tilesToCheckRight = getRightCastleTiles(x, y);
            if (!isTileAttacked(false, tilesToCheckRight) && isTileFree(tilesToCheckRight)) {
                possibleMoves.push_back(&_board.getTileAt(x + 2, y));
            }
        }
    } else {
        const auto &[blackRookLeft, blackRookRight] = _board.getBlackRookMoved();
        if (!blackRookLeft) {
            const PieceTiles tilesToCheckLeft = getLeftCastleTiles(x, y);
            if (!isTileAttacked(true, tilesToCheckLeft) && isTileFree(tilesToCheckLeft)) {
                possibleMoves.push_back(&_board.getTileAt(x - 2, y));
            }
        }
        if (!blackRookRight) {
            const PieceTiles tilesToCheckRight = getRightCastleTiles(x, y);
            if (!isTileAttacked(true, tilesToCheckRight) && isTileFree(tilesToCheckRight)) {
                possibleMoves.push_back(&_board.getTileAt(x + 2, y));
            }
        }
    }
    return possibleMoves;
}

template<size_t N>
PieceTiles ChessMoveLogic::getMovesByDirection(const ChessTile &fromTile, const directionArray<N> &directions) const {
    PieceTiles possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    for (const auto &[xDirection, yDirection] : directions) {
        for (int i = 1; i < 8; i++) { // TODO: check loop range so tile valid does not have to be checked
            const int nextX = x + i * xDirection;
            const int nextY = y + i * yDirection;

            if (!ChessBoard::validTilePos(nextX, nextY)) break;
            const ChessTile &nextTile = _board.getTileAt(nextX, nextY);
            // either it reaches the end or movement gets blocked
            if (!addIfLegalMove(fromTile, nextTile, possibleMoves)) break;
        }
    }
    return possibleMoves;
}

template<size_t N>
PieceTiles ChessMoveLogic::getMovesByDirectionSingle(const ChessTile &fromTile, const directionArray<N> &directions) const {
    PieceTiles possibleMoves;
    const int x = fromTile.getX();
    const int y = fromTile.getY();
    for (const auto &[xDirection, yDirection] : directions) {
        const int nextX = x + xDirection;
        const int nextY = y + yDirection;

        if (!ChessBoard::validTilePos(nextX, nextY)) continue;
        const ChessTile &nextTile = _board.getTileAt(nextX, nextY);
        addIfLegalMove(fromTile, nextTile, possibleMoves);
    }
    return possibleMoves;
}

PieceTiles ChessMoveLogic::getAllLegalToTiles(const bool white) const {
    PieceTiles allPossibleMoves;
    const PieceTiles pieceTiles = _board.getAllTiles(white);
    for (const ChessTile *tile : pieceTiles) {
        PieceTiles newPossibleMoves = getMovesNoCastling(*tile);
        filterLegalMoves(*tile, newPossibleMoves);
        ChessBoard::mergePossVec(allPossibleMoves, newPossibleMoves);
    }
    return allPossibleMoves;
}

// TODO improve if I want to with better getAll.. but for single pieces not all
// of them
PieceTiles ChessMoveLogic::getAllLegalMovesPiece(const bool white, const ChessPieceType piece) const {
    PieceTiles allPossibleMoves;
    const PieceTiles colorPieceTiles = _board.getAllTiles(white);
    for (const ChessTile *tile : colorPieceTiles) {
        if (tile->hasPiece(piece)) {
            ChessBoard::mergePossVec(allPossibleMoves, getMoves(*tile));
        }
    }
    return allPossibleMoves;
}

PieceMoves ChessMoveLogic::getAllLegalMoves(bool white) const {
    PieceMoves allPossMoves;
    const PieceTiles allPieces = _board.getAllTiles(white);
    for (const ChessTile *fromTile : allPieces) {
        const PieceTiles possMoves = getLegalMoves(*fromTile);
        for (const ChessTile *toTile : possMoves) {
            allPossMoves.emplace_back(fromTile, toTile);
        }
    }
    return allPossMoves;
}

std::map<ChessTile, PieceTiles> ChessMoveLogic::getAllLegalMovesMap(bool white) const {
    std::map<ChessTile, PieceTiles> allPossibleMoves;
    const PieceTiles pieceTiles = _board.getAllTiles(white);
    for (const ChessTile *fromMove : pieceTiles) {
        PieceTiles possMoves = getMoves(*fromMove);
        filterLegalMoves(*fromMove, possMoves);
        allPossibleMoves[*fromMove] = possMoves;
    }
    return allPossibleMoves;
}

bool ChessMoveLogic::isInputMovePossible(const ChessTile &fromTile, const ChessTile &toTile) const {
    // get the possible moves for the from tile
    PieceTiles possibleMoves = getMoves(fromTile);
    filterLegalMoves(fromTile, possibleMoves);
    // check if to tile is in those possible moves
    const auto itPossMove = std::ranges::find(possibleMoves, &toTile);
    return itPossMove != possibleMoves.end();
}

void ChessMoveLogic::filterLegalMoves(const ChessTile &fromTile, PieceTiles &possibleMoves) const {
    // conditions are:
    // -- King can not be checked after the move
    // -- Casteling is only allowed if the tiles in in between the king and the rook are not attacked and the king is not checked
    std::erase_if(possibleMoves, [this, &fromTile](const ChessTile *toTile) {
        // check castling move. Free tiles and attacked tiles already checked when getting the moves
        if (isCastling(fromTile, *toTile)) {
            return isKingChecked(_board.isWhitesTurn());
        }
        return isKingCheckedAfterMove(fromTile, *toTile);
    });
}

// returns true if it is a free tile
bool ChessMoveLogic::addIfLegalMove(const ChessTile &fromTile, const ChessTile &toTile, PieceTiles &possibleMoves) {
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
bool ChessMoveLogic::isTileAttacked(const bool white, const PieceTiles &tilesToCheck) const {
    const PieceTiles pieceTiles = white ? _board.getAllWhiteTiles() : _board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        const PieceTiles possMoves = getMovesNoCastling(*tile);
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
bool ChessMoveLogic::isKingChecked(const bool white) const {
    const PieceTiles pieceTiles = _board.getAllTiles(!white);
    for (const ChessTile *tile : pieceTiles) {
        const PieceTiles possMoves = getMovesNoCastling(*tile);
        const bool found = std::ranges::any_of(
            possMoves, [white](const ChessTile *possMove) { return possMove->hasPiece(KING) && possMove->hasWhitePiece() == white; });
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
bool ChessMoveLogic::isKingCheckedAfterMove(const ChessTile &fromTile, const ChessTile &toTile) const {
    ChessBoard checkBoard(_board);
    ChessMoveLogic checkMoveLogic(checkBoard);
    ChessTile &checkFromTile = checkBoard.getTileAt(fromTile.getX(), fromTile.getY());
    ChessTile &checkToTile = checkBoard.getTileAt(toTile.getX(), toTile.getY());
    checkBoard.move(checkFromTile, checkToTile);

    return checkMoveLogic.isKingChecked(!checkBoard.isWhitesTurn());
}

bool ChessMoveLogic::isKingCheckmate(const bool white) const {
    return std::ranges::all_of(_board.getAllTiles(white), [this](const ChessTile *tile) { return getLegalMoves(*tile).empty(); });
}

bool ChessMoveLogic::isDraw(const bool white) const {
    // if (isThreefoldRepetition()) // seems optional so deactivate for now
    // return true; // self explaining :)
    if (getAllLegalToTiles(white).size() <= 0 && !isKingChecked(white)) {
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
    auto countBoardStr = [this](const std::string &_boardStr) {
        return std::ranges::count(_board.getGameHistory(), _boardStr) >= 3;
    };
    return std::ranges::any_of(_board.getGameHistory(), countBoardStr);
}

bool ChessMoveLogic::isDeadPosition() const {
    const PieceTiles whitePieces = _board.getAllWhiteTiles();
    const PieceTiles blackPieces = _board.getAllBlackTiles();
    const size_t whiteSize = whitePieces.size();
    const size_t blackSize = blackPieces.size();
    if (whiteSize > 2 || blackSize > 2) return false;  // always still possible if more then two pieces
    if (whiteSize == 1 && blackSize == 1) return true; // only kings left

    auto is_Bishop = [](const ChessTile *tile) {
        return tile->hasPiece(BISHOP);
    };

    auto is_Knight = [](const ChessTile *tile) {
        return tile->hasPiece(KNIGHT);
    };

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
