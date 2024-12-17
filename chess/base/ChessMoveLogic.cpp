#include "ChessMoveLogic.h"

ChessMoveLogic::ChessMoveLogic() {}

Pieces ChessMoveLogic::getPossibleMoves(const ChessBoard &board, const ChessTile *fromTile)
{
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

Pieces ChessMoveLogic::getPossibleMovesPawn(const ChessBoard &board, const ChessTile *fromTile)
{
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
    if (board.doublePawnMoveAt.first != -1) {
        if (board.doublePawnMoveAt.second == y) {
            if (x - 1 == board.doublePawnMoveAt.first) {
                // TODO: wanted to know if in the current turn a en passant is possible. Better move somewhere else
                // enPassantPossible = true;
                possibleMoves.push_back(board.getTileAt(x - 1, y + white));
            } else if (x + 1 == board.doublePawnMoveAt.first) {
                // enPassantPossible = true;
                possibleMoves.push_back(board.getTileAt(x + 1, y + white));
            }
        }
    }
    return possibleMoves;
}

inline Pieces ChessMoveLogic::getPossibleMovesBishop(const ChessBoard &board, const ChessTile *fromTile)
{
    static const std::vector directions = {std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirection(board, fromTile, directions);
}

inline Pieces ChessMoveLogic::getPossibleMovesRook(const ChessBoard &board, const ChessTile *fromTile)
{
    static const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    return getPossibleMovesByDirection(board, fromTile, directions);
}

inline Pieces ChessMoveLogic::getPossibleMovesQueen(const ChessBoard &board, const ChessTile *fromTile)
{
    Pieces possibleMoves = getPossibleMovesBishop(board, fromTile);
    ChessBoard::mergePossVec(possibleMoves, getPossibleMovesRook(board, fromTile));
    return possibleMoves;
}

inline Pieces ChessMoveLogic::getPossibleMovesKnight(const ChessBoard &board, const ChessTile *fromTile)
{
    static const std::vector directions = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                           std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    return getPossibleMovesByDirectionSingle(board, fromTile, directions);
}

Pieces ChessMoveLogic::getPossibleMovesKing(const ChessBoard &board, const ChessTile *fromTile, const bool castling,
                                            const bool white)
{
    Pieces possibleMoves;
    static const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                           std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};
    ChessBoard::mergePossVec(possibleMoves, getPossibleMovesByDirectionSingle(board, fromTile, directions));
    if (castling && !isKingChecked(white))
        ChessBoard::mergePossVec(possibleMoves, getPossibleMovesCastling(board,fromTile));
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesByDirection(const ChessBoard &board, const ChessTile *fromTile,
                                               const std::vector<std::pair<int, int>> &directions)
{
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (const auto &[xDirection, yDirection] : directions) {
        for (int i = 1; i < 8; i++) {
            ChessTile *nextTile =board.getTileAt(x + i * xDirection, y + i * yDirection);
            if (!addIfPossibleMove(fromTile, nextTile, possibleMoves))
                break;
        }
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesByDirectionSingle(const ChessBoard &board, const ChessTile *fromTile,
                                                     const std::vector<std::pair<int, int>> &directions)
{
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (const auto &[xDirection, yDirection] : directions) {
        ChessTile *nextTile = board.getTileAt(x + xDirection, y + yDirection);
        addIfPossibleMove(fromTile, nextTile, possibleMoves);
    }
    return possibleMoves;
}

Pieces ChessMoveLogic::getPossibleMovesCastling(const ChessBoard &board, const ChessTile *fromTile)
{
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const Pieces tilesToCheckLeft = {board.getTileAt(x - 1, y), board.getTileAt(x - 2, y)};
    const Pieces tilesToCheckRight = {board.getTileAt(x + 1, y), board.getTileAt(x + 2, y)};
    if (whitesTurn) {
        if (whiteRookMoved.first == false) {
            if (!isTileAttackedAndFree(false, tilesToCheckLeft)) {
                possibleMoves.push_back(getTileAt(x - 2, y));
            }
        }
        if (whiteRookMoved.second == false) {
            if (!isTileAttackedAndFree(false, tilesToCheckRight)) {
                possibleMoves.push_back(getTileAt(x + 2, y));
            }
        }
    } else {
        if (blackRookMoved.first == false) {
            if (!isTileAttackedAndFree(true, tilesToCheckLeft)) {
                possibleMoves.push_back(getTileAt(x - 2, y));
            }
        }
        if (blackRookMoved.second == false) {
            if (!isTileAttackedAndFree(true, tilesToCheckRight)) {
                possibleMoves.push_back(getTileAt(x + 2, y));
            }
        }
    }
    return possibleMoves;
}

bool ChessMoveLogic::addIfPossibleMove(const ChessTile *fromTile, ChessTile *toTile, Pieces &possibleMoves)
{
    if (toTile == nullptr) // tile does not exist
        return false;
    if (toTile->piece != nullptr) { // there is a piece
        if (fromTile->piece->getType() == Pawn &&
            fromTile->getX() == toTile->getX()) { // pawn only piece that can't capture piece
                                                  // directly in its path
            return false;
        }
        if (fromTile->piece->isWhite() != toTile->piece->isWhite()) // piece is different color so add it
            possibleMoves.push_back(toTile);
        return false; // piece is same color so don't add it
    }
    if (fromTile->piece->getType() == Pawn && fromTile->getX() != toTile->getX() &&
        toTile->piece == nullptr) // for that pawn move
        return false;
    possibleMoves.push_back(toTile); // tile is free to move on
    return true;
}

bool ChessMoveLogic::isTileAttacked(const ChessBoard &board, const bool white, const Pieces &tilesToCheck)
{
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