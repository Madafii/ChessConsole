#include "ChessMoveLogic.h"
#include "ChessBoard.h"

#include <algorithm>

ChessMoveLogic::ChessMoveLogic() {}

Pieces ChessMoveLogic::getPossibleMoves(const ChessBoard &board, const ChessTile *fromTile) {
    if (fromTile->piece == nullptr)
        return {};
    switch (fromTile->piece->getType()) {
    case Pawn:
        return getPossibleMovesPawn(board, fromTile);
    case Rook:
        return getPossibleMovesRook(board, fromTile);
    case Knight:
        return getPossibleMovesKnight(board, fromTile);
    case Bishop:
        return getPossibleMovesBishop(board, fromTile);
    case Queen:
        return getPossibleMovesQueen(board, fromTile);
    case King:
        return getPossibleMovesKing(board, fromTile);
    default:
        return {};
    }
}

Pieces ChessMoveLogic::getPossibleMovesPawn(const ChessBoard &board, const ChessTile *fromTile) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const int white = fromTile->piece->isWhite() ? 1 : -1;
    // some different rules for pawn applying again
    if (addIfPossibleMove(fromTile, board.getTileAt(x, y + white), possibleMoves)) {
        if (white == 1 && y == 1) {
            addIfPossibleMove(fromTile, board.getTileAt(x, y + 2 * white), possibleMoves);
        }
        if (white == -1 && y == 6) {
            addIfPossibleMove(fromTile, board.getTileAt(x, y + 2 * white), possibleMoves);
        }
    }
    // capture other pawn
    addIfPossibleMove(fromTile, board.getTileAt(x + 1, y + white), possibleMoves);
    addIfPossibleMove(fromTile, board.getTileAt(x - 1, y + white), possibleMoves);
    // get pawn move for that special pawn move that never happens
    if (board.doublePawnMoveAt.second == y) {
        if (x - 1 == board.doublePawnMoveAt.first) {
            // TODO: wanted to know if in the current turn a en passant is possible. Better move
            // somewhere else enPassantPossible = true;
            possibleMoves.push_back(board.getTileAt(x - 1, y + white));
        } else if (x + 1 == board.doublePawnMoveAt.first) {
            possibleMoves.push_back(board.getTileAt(x + 1, y + white));
        }
    }
    return possibleMoves;
}

inline Pieces ChessMoveLogic::getPossibleMovesBishop(const ChessBoard &board, const ChessTile *fromTile) {
    static const std::vector directions = {std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirection(board, fromTile, directions);
}

inline Pieces ChessMoveLogic::getPossibleMovesKnight(const ChessBoard &board, const ChessTile *fromTile) {
    static const std::vector directions = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                           std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    return getPossibleMovesByDirectionSingle(board, fromTile, directions);
}

inline Pieces ChessMoveLogic::getPossibleMovesRook(const ChessBoard &board, const ChessTile *fromTile) {
    static const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    return getPossibleMovesByDirection(board, fromTile, directions);
}

inline Pieces ChessMoveLogic::getPossibleMovesQueen(const ChessBoard &board, const ChessTile *fromTile) {
    Pieces possibleMoves = getPossibleMovesBishop(board, fromTile);
    ChessBoard::mergePossVec(possibleMoves, getPossibleMovesRook(board, fromTile));
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesKing(const ChessBoard &board, const ChessTile *fromTile, const bool castling) {
    static const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                           std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};
    Pieces possibleMoves = getPossibleMovesByDirectionSingle(board, fromTile, directions);
    if (castling && !isKingChecked(board, board.whitesTurn))
        ChessBoard::mergePossVec(possibleMoves, getPossibleMovesCastling(board, fromTile));
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesCastling(const ChessBoard &board, const ChessTile *fromTile) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const Pieces tilesToCheckLeft = {board.getTileAt(x - 1, y), board.getTileAt(x - 2, y)};
    const Pieces tilesToCheckRight = {board.getTileAt(x + 1, y), board.getTileAt(x + 2, y)};
    if (board.whitesTurn) {
        if (board.whiteRookMoved.first == false) {
            if (!isTileAttacked(board, false, tilesToCheckLeft) && isTileFree(board, tilesToCheckLeft)) {
                possibleMoves.push_back(board.getTileAt(x - 2, y));
            }
        }
        if (board.whiteRookMoved.second == false) {
            if (!isTileAttacked(board, false, tilesToCheckRight) && isTileFree(board, tilesToCheckRight)) {
                possibleMoves.push_back(board.getTileAt(x + 2, y));
            }
        }
    } else {
        if (board.blackRookMoved.first == false) {
            if (!isTileAttacked(board, false, tilesToCheckLeft) && isTileFree(board, tilesToCheckLeft)) {
                possibleMoves.push_back(board.getTileAt(x - 2, y));
            }
        }
        if (board.blackRookMoved.second == false) {
            if (!isTileAttacked(board, false, tilesToCheckRight) && isTileFree(board, tilesToCheckRight)) {
                possibleMoves.push_back(board.getTileAt(x + 2, y));
            }
        }
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesByDirection(const ChessBoard &board, const ChessTile *fromTile,
                                                   const std::vector<std::pair<int, int>> &directions) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (const auto &[xDirection, yDirection] : directions) {
        for (int i = 1; i < 8; i++) {
            ChessTile *nextTile = board.getTileAt(x + i * xDirection, y + i * yDirection);
            if (!addIfPossibleMove(fromTile, nextTile, possibleMoves))
                break;
        }
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesByDirectionSingle(const ChessBoard &board, const ChessTile *fromTile,
                                                         const std::vector<std::pair<int, int>> &directions) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (const auto &[xDirection, yDirection] : directions) {
        ChessTile *nextTile = board.getTileAt(x + xDirection, y + yDirection);
        addIfPossibleMove(fromTile, nextTile, possibleMoves);
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getAllPossibleMoves(const ChessBoard &board, const bool white) {
    Pieces allPossibleMoves;
    const Pieces pieceTiles = white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        Pieces newPossibleMoves;
        if (tile->piece->getType() == King) {
            newPossibleMoves = getPossibleMovesKing(board, tile, false);
        } else {
            newPossibleMoves = getPossibleMoves(board, tile);
        }
        filterPossibleMovesForChecks(board, tile, newPossibleMoves);
        ChessBoard::mergePossVec(allPossibleMoves, newPossibleMoves);
    }
    return allPossibleMoves;
}

inline void ChessMoveLogic::filterPossibleMovesForChecks(const ChessBoard &board, const ChessTile *fromTile, Pieces &possibleMoves) {
    std::erase_if(possibleMoves, [&board, &fromTile](ChessTile *toTile) { return isKingChecked(board, fromTile, toTile); });
}

bool ChessMoveLogic::isInputMovePossible(const ChessBoard &board, const ChessTile *fromTile, const ChessTile *toTile) {
    // get the possible moves for the from tile
    Pieces possibleMoves = getPossibleMoves(board, fromTile);
    filterPossibleMovesForChecks(board, fromTile, possibleMoves);
    // check if to tile is in those possible moves
    const auto itPossMove = std::ranges::find(possibleMoves, toTile);
    if (itPossMove == possibleMoves.end()) {
        std::cout << "ChessMoveLogic::isInputMovePossible: that is not a possible move" << std::endl;
        return false;
    }
    return true;
}

bool ChessMoveLogic::addIfPossibleMove(const ChessTile *fromTile, ChessTile *toTile, Pieces &possibleMoves) {
    if (toTile == nullptr) // tile does not exist
        return false;
    if (toTile->piece != nullptr) {                                                     // there is a piece
        if (fromTile->piece->getType() == Pawn && fromTile->getX() == toTile->getX()) { // pawn only piece that can't capture piece
                                                                                        // directly in its path
            return false;
        }
        if (fromTile->piece->isWhite() != toTile->piece->isWhite()) // piece is different color so add it
            possibleMoves.push_back(toTile);
        return false; // piece is same color so don't add it
    }
    if (fromTile->piece->getType() == Pawn && fromTile->getX() != toTile->getX() && toTile->piece == nullptr) // for that pawn move
        return false;
    possibleMoves.push_back(toTile); // tile is free to move on
    return true;
}

// check if tile attacked by @param white that color
bool ChessMoveLogic::isTileAttacked(const ChessBoard &board, const bool white, const Pieces &tilesToCheck) {
    const Pieces pieceTiles = white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        Pieces possMoves;
        // don't check castling otherwise will loop endlessly
        if (tile->piece->getType() == King) {
            possMoves = getPossibleMovesKing(board, tile, false);
        } else {
            possMoves = getPossibleMoves(board, tile);
        }
        for (const ChessTile *tileToCheck : tilesToCheck) {
            if (std::ranges::find(possMoves, tileToCheck) != possMoves.end()) {
                return true;
            }
        }
    }
    return false;
}

bool ChessMoveLogic::isTileFree(const ChessBoard &board, const Pieces &tilesToCheck) {
    for (const ChessTile *tileToCheck : tilesToCheck) {
        if (tileToCheck->piece != nullptr)
            return false;
    }
    return true;
}

///
/// @param white The color to check for being checked
/// @return if white is checked
bool ChessMoveLogic::isKingChecked(const ChessBoard &board, const bool white) {
    const Pieces pieceTiles = !white ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        Pieces possMoves;
        if (tile->piece->getType() == King)
            possMoves = getPossibleMovesKing(board, tile, false);
        else
            possMoves = getPossibleMoves(board, tile);
        for (const ChessTile *possMove : possMoves) {
            if (possMove->piece == nullptr)
                continue;
            if (possMove->piece->getType() == King && possMove->piece->isWhite() == white) {
                return true;
            }
        }
    }
    return false;
}

///
/// @param fromTile the tile to move from
/// @param toTile the tile to move to
/// @return if King is checked after that move
bool ChessMoveLogic::isKingChecked(const ChessBoard &board, const ChessTile *fromTile, ChessTile *toTile) {
    // make the move then check the board.
    const bool white = fromTile->piece->isWhite();
    // that special move happened so extra rule with capturing
    bool resetEnPassant = false;
    ChessTile *enPassantTile = nullptr;
    std::unique_ptr<ChessPiece> enPassantPiece = nullptr;
    if (fromTile->piece->getType() == Pawn && fromTile->getX() != toTile->getX() && toTile->piece == nullptr) {
        resetEnPassant = true;
        const int whiteMove = board.whitesTurn ? -1 : 1;
        enPassantTile = board.getTileAt(toTile->getX(), toTile->getY() + whiteMove);
        if (enPassantTile->piece == nullptr) { // TODO: stupid workaround idk if it really works all the time.
            enPassantTile = board.getTileAt(toTile->getX(), toTile->getY() - whiteMove);
        }
        enPassantPiece = std::move(enPassantTile->piece);
    }
    std::unique_ptr<ChessPiece> tmpPieceTo = std::move(toTile->piece);
    auto *tmpTile = const_cast<ChessTile *>(fromTile);
    toTile->piece = std::move(tmpTile->piece);
    const bool isChecked = isKingChecked(board, white);
    tmpTile->piece = std::move(toTile->piece);
    toTile->piece = std::move(tmpPieceTo);
    if (resetEnPassant) {
        enPassantTile->piece = std::move(enPassantPiece);
    }
    return isChecked;
}

bool ChessMoveLogic::isKingCheckmate(const ChessBoard &board) {
    const Pieces pieceTiles = !board.whitesTurn ? board.getAllWhiteTiles() : board.getAllBlackTiles();
    for (const ChessTile *tile : pieceTiles) {
        Pieces possMoves;
        if (tile->piece->getType() == King) {
            possMoves = getPossibleMovesKing(board, tile, false);
        } else {
            possMoves = getPossibleMoves(board, tile);
        }
        filterPossibleMovesForChecks(board, tile, possMoves);
        if (!possMoves.empty()) {
            return false;
        }
    }
    return true;
}

bool ChessMoveLogic::isDraw(const ChessBoard &board) {
    // if (isThreefoldRepetition()) // seems optional so deactivate for now
    // return true; // self explaining :)
    if (getAllPossibleMoves(board, !board.whitesTurn).size() <= 0 && !isKingChecked(board, !board.whitesTurn))
        return true; // stalemate
    // if (movesSinceLastCapture > 100) // maybe that is optional too but leave like this for now
    // return true; // 100 half turns with no capture or pawn move
    // check for any other dead position where a win is not possible anymore
    if (isDeadPosition(board)) {
        return true;
    }
    return false;
}

bool ChessMoveLogic::isThreefoldRepetition(const ChessBoard &board) {
    auto countBoardStr = [&board](const std::string &boardStr) { return std::ranges::count(board.gameHistory, boardStr) >= 3; };
    return std::ranges::any_of(board.gameHistory, countBoardStr);
}

bool ChessMoveLogic::isDeadPosition(const ChessBoard &board) {
    const Pieces whitePieces = board.getAllWhiteTiles();
    const Pieces blackPieces = board.getAllBlackTiles();
    const size_t whiteSize = whitePieces.size();
    const size_t blackSize = blackPieces.size();
    if (whiteSize > 2 || blackSize > 2)
        return false; // always still possible if more then two pieces
    if (whiteSize == 1 && blackSize == 1)
        return true; // only kings left

    auto is_Bishop = [](const ChessTile *tile) {
        if (tile->piece->getType() == Bishop) {
            return true;
        }
        return false;
    };

    auto is_Knight = [](const ChessTile *tile) {
        if (tile->piece->getType() == Knight) {
            return true;
        }
        return false;
    };

    const auto itWhiteBishop = std::ranges::find_if(whitePieces, is_Bishop);
    const auto itBlackBishop = std::ranges::find_if(blackPieces, is_Bishop);
    const auto itWhiteKnight = std::ranges::find_if(whitePieces, is_Knight);
    const auto itBlackKnight = std::ranges::find_if(blackPieces, is_Knight);
    if (whitePieces.size() == 2 && blackPieces.size() == 2) {
        // only if both are bishop and they are on the same color tile
        if (itWhiteBishop == whitePieces.end())
            return false;
        if (itBlackBishop == blackPieces.end())
            return false;
        if ((*itWhiteBishop)->piece->isWhite() == (*itBlackBishop)->piece->isWhite())
            return true;
        return false;
    }
    // if the only other piece in the game is one of them also a dead position
    // draw
    if (itWhiteBishop != whitePieces.end() || itWhiteKnight != whitePieces.end() || itBlackBishop != blackPieces.end() ||
        itBlackKnight != blackPieces.end())
        return true;
    return false;
}
