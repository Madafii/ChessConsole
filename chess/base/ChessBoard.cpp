#include "ChessBoard.h"

#include <algorithm>
#include <cstdlib>
#include <string>
#include <utility>

void ChessBoard::initBoard() {
    board[0] = ChessTile(ChessPiece(ChessPieceType::ROOK, true), 0, 0);
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

// get a simple string of the current board
std::string ChessBoard::getStringFromBoard() const {
    std::string outMoves;
    for (const auto &tile : board) {
        if (tile.hasPiece(ChessPieceType::NONE)) {
            outMoves += "_";
            continue;
        }
        if (tile.hasWhitePiece()) {
            // white pieces in lower case
            outMoves += static_cast<char>(std::tolower(tile.getPiece().getShortName()));
            continue;
        }
        // black pieces in upper case
        outMoves += tile.getPiece().getShortName();
    }
    // TODO: thinking of writing a separate function to know if a en Passant is possible actual why I have this?
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
        if (tile.hasPiece(ChessPieceType::NONE)) continue;
        if (tile.getPiece().isWhite()) whiteTiles.push_back(&tile);
    }
    return whiteTiles;
}

Pieces ChessBoard::getAllBlackTiles() const {
    Pieces blackTiles;
    for (auto &tile : board) {
        if (tile.hasPiece(ChessPieceType::NONE)) continue;
        if (!tile.getPiece().isWhite()) blackTiles.push_back(&tile);
    }
    return blackTiles;
}

Pieces ChessBoard::getWhitePieceType(const ChessPieceType piece) const {
    Pieces whitePiecesType;
    for (auto &tile : board) {
        if (tile.hasPiece(ChessPieceType::NONE)) continue;
        if (tile.getPiece().isWhite() && tile.getPiece().getType() == piece) {
            whitePiecesType.push_back(&tile);
        }
    }
    return whitePiecesType;
}

Pieces ChessBoard::getBlackPieceType(const ChessPieceType piece) const {
    Pieces blackPiecesType;
    for (auto &tile : board) {
        if (tile.hasPiece(ChessPieceType::NONE)) continue;
        if (!tile.getPiece().isWhite() && tile.getPiece().getType() == piece) {
            blackPiecesType.push_back(&tile);
        }
    }
    return blackPiecesType;
}

Pieces ChessBoard::getAllPiecesFor(const bool white, const ChessPieceType piece) const {
    Pieces pieces;
    const Pieces colorPieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile : colorPieceTiles) {
        if (tile->hasPiece(piece)) {
            pieces.push_back(tile);
        }
    }
    return pieces;
}

// every actual board piece change happens here
void ChessBoard::move(ChessTile &fromTile, ChessTile &toTile, const char pawnToPiece) {
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

    fromTile.occupyPiece(toTile.getPiece());

    if (isPawnWinCondition(toTile)) pawnWon(toTile, pawnToPiece);

    endMove();
}

void ChessBoard::endMove() {
    if (getEnPassantMarker() != isWhitesTurn()) {
        setEnPassantPossible(false);
        resetLastDoublePawnMove();
    }
    pushGameToHistory();
    toggleTurn();
}

void ChessBoard::movePawn(const ChessTile &fromTile, const ChessTile &toTile) {
    // for that special pawn movement check if pawn moved more than two tiles.
    if (abs(fromTile.getY() - toTile.getY()) >= 2) {
        setLastDoublePawnMove({toTile.getX(), toTile.getY()});
        setEnPassantMarker(whitesTurn); // mark this turn for enPassant
    }
    // that special move happened so extra rule with capturing
    if (fromTile.getX() != toTile.getX() && toTile.getPiece().getType() == ChessPieceType::NONE) {
        const int whiteMove = whitesTurn ? -1 : 1;
        ChessTile &capturedPiece = getTileAt(toTile.getX(), toTile.getY() + whiteMove);
        capturedPiece.changePiece(ChessPiece());
    }
}

void ChessBoard::moveKing(const ChessTile &fromTile, const ChessTile &toTile) {
    // disables castling when the king is being moved
    if (fromTile.getPiece().isWhite() == true) {
        whiteRookMoved = {true, true};
    } else {
        blackRookMoved = {true, true};
    }
    // do castling move for the rook
    if (abs(fromTile.getX() - toTile.getX()) >= 2) {
        const int x = toTile.getX();
        const int y = toTile.getY();
        if (x < 4) {
            // castle happened on left side
            ChessTile &rookTile = getTileAt(0, y);
            ChessTile &rookToTile = getTileAt(3, y);
            rookToTile.switchPiece(rookTile.getPiece());
        } else {
            // castle happened on right side
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
    } else if (x == 7) {
        if (y == 0) whiteRookMoved.second = true;
        if (y == 7) blackRookMoved.second = true;
    }
}

bool ChessBoard::validTilePos(std::string_view pos) {
    if (pos.size() != 2) {
        return false;
    }
    const char xText = pos[0];
    if (xText != std::clamp(xText, 'a', 'h')) return false;
    const int x = ChessTile::mapXtoInt.at(xText);
    const int y = pos[1] - '0'; // is a trick to convert number char to int
    return validTilePos(x, y);
}

void ChessBoard::pawnWon(ChessTile &pawnTile, const char pawnToPiece) {
    const bool white = pawnTile.getPiece().isWhite();
    switch (pawnToPiece) {
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
            break;
    }
}

void ChessBoard::mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge) {
    possibleMoves.insert(possibleMoves.end(), possibleMovesMerge.begin(), possibleMovesMerge.end());
}
