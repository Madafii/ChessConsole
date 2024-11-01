//
// Created by fpittermann on 10/5/24.
//

#include "ChessBoard.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

ChessBoard::ChessBoard() {
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
        std::cout << "\x1b[0m" << std::endl; // Reset to default color after each line
    }
    std::cout << "    a  b  c  d  e  f  g  h " << std::endl;
}

Pieces ChessBoard::getPossibleMoves(const ChessTile *fromTile) {
    Pieces possibleMoves;
    if (fromTile->piece == nullptr)
        return possibleMoves;
    switch (fromTile->piece->getType()) {
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

Pieces ChessBoard::getPossibleMovesPawn(const ChessTile *fromTile) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const int white = fromTile->piece->isWhite() ? 1 : -1;
    // some different rules for pawn applying again
    if (isPossibleMove(fromTile, getTileAt(x, y + white), possibleMoves)) {
        if (white == 1 && y == 1) {
            isPossibleMove(fromTile, getTileAt(x, y + 2 * white), possibleMoves);
        }
        if (white == -1 && y == 6) {
            isPossibleMove(fromTile, getTileAt(x, y + 2 * white), possibleMoves);
        }
    }
    // capture other pawn
    isPossibleMove(fromTile, getTileAt(x + 1, y + white), possibleMoves);
    isPossibleMove(fromTile, getTileAt(x - 1, y + white), possibleMoves);
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

Pieces ChessBoard::getPossibleMovesBishop(const ChessTile *fromTile) {
    const std::vector directions = {std::pair(1, 1), std::pair(1, -1), std::pair(-1, 1), std::pair(-1, -1)};
    return getPossibleMovesByDirection(fromTile, directions);
}

Pieces ChessBoard::getPossibleMovesRook(const ChessTile *fromTile) {
    const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(-1, 0), std::pair(1, 0)};
    return getPossibleMovesByDirection(fromTile, directions);
}

Pieces ChessBoard::getPossibleMovesQueen(const ChessTile *fromTile) {
    Pieces possibleMoves = getPossibleMovesBishop(fromTile);
    mergePossVec(possibleMoves, getPossibleMovesRook(fromTile));
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesKnight(const ChessTile *fromTile) {
    const std::vector directions = {std::pair(2, 1), std::pair(2, -1), std::pair(-2, 1), std::pair(-2, -1),
                                    std::pair(1, 2), std::pair(-1, 2), std::pair(1, -2), std::pair(-1, -2)};
    return getPossibleMovesByDirectionSingle(fromTile, directions);
}

Pieces ChessBoard::getPossibleMovesKing(const ChessTile *fromTile, const bool castling) {
    Pieces possibleMoves;
    const std::vector directions = {std::pair(0, 1), std::pair(0, -1), std::pair(1, 1),  std::pair(1, -1),
                                    std::pair(1, 0), std::pair(-1, 0), std::pair(-1, 1), std::pair(-1, -1)};
    mergePossVec(possibleMoves, getPossibleMovesByDirectionSingle(fromTile, directions));
    if (castling && !isKingChecked(whitesTurn))
        mergePossVec(possibleMoves, getPossibleMovesCastling(fromTile));
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesByDirection(const ChessTile *fromTile,
                                               const std::vector<std::pair<int, int>> &directions) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (std::pair pair: directions) {
        const int xDirection = pair.first;
        const int yDirection = pair.second;
        for (int i = 1; i < 8; i++) {
            ChessTile *nextTile = getTileAt(x + i * xDirection, y + i * yDirection);
            if (!isPossibleMove(fromTile, nextTile, possibleMoves))
                break;
        }
    }
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesByDirectionSingle(const ChessTile *fromTile,
                                                     const std::vector<std::pair<int, int>> &directions) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    for (std::pair pair: directions) {
        const int xDirection = pair.first;
        const int yDirection = pair.second;
        ChessTile *nextTile = getTileAt(x + xDirection, y + yDirection);
        isPossibleMove(fromTile, nextTile, possibleMoves);
    }
    return possibleMoves;
}

Pieces ChessBoard::getPossibleMovesCastling(const ChessTile *fromTile) {
    Pieces possibleMoves;
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    const Pieces tilesToCheckLeft = {
        getTileAt(x - 1, y),
        getTileAt(x - 2, y),
        getTileAt(x - 3, y)
    };
    const Pieces tilesToCheckRight = {
        getTileAt(x + 1, y),
        getTileAt(x + 2, y)
    };
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
    }
    else {
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

Pieces ChessBoard::getAllWhiteTiles() const {
    Pieces whiteTiles;
    for (ChessTile *tile: board) {
        if (tile->piece == nullptr)
            continue;
        if (tile->piece->isWhite())
            whiteTiles.push_back(tile);
    }
    return whiteTiles;
}

Pieces ChessBoard::getAllBlackTiles() const {
    Pieces blackTiles;
    for (ChessTile *tile: board) {
        if (tile->piece == nullptr)
            continue;
        if (!tile->piece->isWhite())
            blackTiles.push_back(tile);
    }
    return blackTiles;
}

void ChessBoard::filterPossibleMovesForChecks(const ChessTile *fromTile, Pieces &possibleMoves) {
    // std::cout << "possible Moves size before filter: " << possibleMoves.size() << std::endl;
    // check if the move would make the player checked which is an illegal move
    std::erase_if(possibleMoves, [fromTile, this](ChessTile *toTile) { return isKingChecked(fromTile, toTile); });
    // std::cout << "possible moves size after filter: " << possibleMoves.size() << std::endl;
}

bool ChessBoard::isInputMovePossible(const ChessTile *fromTile, const ChessTile *toTile) {
    Pieces possibleMoves = getPossibleMoves(fromTile);
    filterPossibleMovesForChecks(fromTile, possibleMoves);
    // just for debugging
    std::cout << "the possible moves are:" << std::endl;
    for (const auto possMove: possibleMoves) {
        std::cout << possMove->getX() << " " << possMove->getY() << std::endl;
    }
    const auto itPossMove = std::ranges::find(possibleMoves, toTile);
    if (itPossMove == possibleMoves.end()) {
        std::cout << "ChessBoard::isInputMovePossible: that is not a possible move" << std::endl;
        return false;
    }
    return true;
}

bool ChessBoard::isPossibleMove(const ChessTile *fromTile, ChessTile *toTile, Pieces &possibleMoves) {
    if (toTile == nullptr) // tile does not exist
        return false;
    if (toTile->piece != nullptr) { // there is a piece
        if (fromTile->piece->getType() == Pawn && fromTile->getX() == toTile->getX()) { // pawn only piece that can't capture piece directly in its path
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

///
/// @param white The color to check for being checked
/// @return if white is checked
bool ChessBoard::isKingChecked(const bool white) {
    const Pieces pieceTiles = !white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile: pieceTiles) {
        Pieces possMoves;
        if (tile->piece->getType() == King)
            possMoves = getPossibleMovesKing(tile, false);
        else
            possMoves = getPossibleMoves(tile);
        for (const ChessTile *possMove: possMoves) {
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
bool ChessBoard::isKingChecked(const ChessTile *fromTile, ChessTile *toTile) {
    // make the move then check the board.
    const bool white = fromTile->piece->isWhite();
    ChessPiece *tmpPieceFrom = fromTile->piece;
    ChessPiece *tmpPieceTo = toTile->piece;
    ChessTile *tmpFromTile =  const_cast<ChessTile*> (fromTile); // ok to do this because temporary? ...idk
    tmpFromTile->piece = nullptr;
    toTile->piece = tmpPieceFrom;
    const bool isChecked = isKingChecked(white);
    tmpFromTile->piece = tmpPieceFrom;
    toTile->piece = tmpPieceTo;
    return isChecked;
}

// if passed tiles are attacked by white
bool ChessBoard::isTileAttackedAndFree(const bool white, const Pieces &tilesToCheck) {
    for (const ChessTile *tileToCheck : tilesToCheck) {
        if (tileToCheck->piece != nullptr) return true;
    }
    const Pieces pieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile: pieceTiles) {
        Pieces possMoves;
        // don't check castling otherwise will loop endlessly
        if (tile->piece->getType() == King) {
            possMoves = getPossibleMovesKing(tile, false);
        } else {
            possMoves = getPossibleMoves(tile);
        }
        for (const ChessTile *tileToCheck : tilesToCheck) {
            if (std::ranges::find(possMoves, tileToCheck) != possMoves.end()) {
                return true;
            }
        }
    }
    return false;
}

bool ChessBoard::isKingCheckmate() {
    const Pieces pieceTiles = !whitesTurn ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile: pieceTiles) {
        Pieces possMoves;
        if (tile->piece->getType() == King) {
            possMoves = getPossibleMovesKing(tile, false);
        } else {
            possMoves = getPossibleMoves(tile);
        }
        filterPossibleMovesForChecks(tile, possMoves);
        if (possMoves.size() > 0) {
            std::cout << tile->piece->getShortName() << "at: " << tile->getX() << " : " << tile->getY() << " still has possible Moves" << std::endl;
            return false;
        }
    }
    std::cout << "yeah you won!" << std::endl;
    return true;
}
bool ChessBoard::isDraw() {
    // TODO: draw when same position repeats three times
    if (getAllPossibleMoves(!whitesTurn).size() <= 0 && !isKingChecked(!whitesTurn)) return true;
    if (movesSinceLastCapture >= 100) return true;
    return false;
}

Pieces ChessBoard::getAllPossibleMoves(const bool white) {
    Pieces allPossibleMoves;
    const Pieces pieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile: pieceTiles) {
        Pieces newPossibleMoves;
        if (tile->piece->getType() == King) {
            newPossibleMoves = getPossibleMovesKing(tile, false);
        } else {
            newPossibleMoves = getPossibleMoves(tile);
        }
        filterPossibleMovesForChecks(tile, newPossibleMoves);
        mergePossVec(allPossibleMoves, newPossibleMoves);
    }
    return allPossibleMoves;
}

// TODO improve if I want to with better getAll.. but for single pieces not all of them
Pieces ChessBoard::getAllPossibleMovesPiece(const bool white, const ChessPieceType piece) {
    Pieces allPossibleMoves;
    const Pieces colorPieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (const ChessTile *tile: colorPieceTiles) {
        if (tile->piece->getType() == piece) {
            mergePossVec(allPossibleMoves, getPossibleMoves(tile));
        }
    }
    return allPossibleMoves;
}

Pieces ChessBoard::getAllPiecesFor(const bool white, const ChessPieceType piece) const {
    Pieces pieces;
    const Pieces colorPieceTiles = white ? getAllWhiteTiles() : getAllBlackTiles();
    for (ChessTile *tile: colorPieceTiles) {
        if (tile->piece->getType() == piece) {
            pieces.push_back(tile);
        }
    }
    return pieces;
}

GameState ChessBoard::handleMoveInput(std::string input) {
    char pawnChangeTo = '0'; // 0 is for no pawn change
    // extra check for instant pawn change
    if (input.length() == 7) {
        pawnChangeTo = input[6];
    }
    input = input.substr(0, 5);

    // get the move tiles from the input
    const auto moveTilePair = getMoveTilesFromInput(input);
    if (!moveTilePair) {
        std::cout << "ChessBoard::handleMoveInput: invalid input: " << input << std::endl;
    }
    ChessTile *fromTile = moveTilePair->first;
    ChessTile *toTile = moveTilePair->second;

    if (!isInputMovePossible(fromTile, toTile)) return GameState::IN_PROGRESS;
    move(fromTile, toTile);

    // after move checks
    const GameState game_state = afterMoveChecks(toTile, pawnChangeTo);

    // nothing else happening here because updating the screen
    updateBoard();
    return game_state;
}

void ChessBoard::move(ChessTile *fromTile, ChessTile *toTile) {
    // some special rules for pawns
    if (fromTile->piece->getType() == Pawn)
        movePawn(fromTile, toTile);
    // some special rules for rooks
    else if (fromTile->piece->getType() == Rook) {
        moveRook(fromTile);
    }
    // some special rules for kings
    else if (fromTile->piece->getType() == King) {
        moveKing(fromTile, toTile);
    }
    if (toTile->piece != nullptr) {
        movesSinceLastCapture = 0;
        delete toTile->piece;
    } else if (fromTile->piece->getType() == Pawn) {
        movesSinceLastCapture = 0;
    } else {
        movesSinceLastCapture++;
    }
    toTile->piece = fromTile->piece;
    fromTile->piece = nullptr;
}

void ChessBoard::movePawn(const ChessTile *fromTile, const ChessTile *toTile) {
    // for that special pawn movement check if pawn moved more than two tiles.
    if (abs(fromTile->getY() - toTile->getY()) >= 2) {
        doublePawnMoveAt = toTile->getX();
    }
    // that special move happened so extra rule with capturing
    if (fromTile->getX() != toTile->getX() && toTile->piece == nullptr) {
        const int whiteMove = whitesTurn ? -1 : 1;
        ChessTile *capturedPiece = getTileAt(toTile->getX(), toTile->getY() + whiteMove);
        delete capturedPiece->piece;
        capturedPiece->piece = nullptr;
    }
}

void ChessBoard::moveKing(const ChessTile *fromTile, const ChessTile *toTile) {
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
        }
        else {
            rookTile = getTileAt(7, y);
            rookToTile = getTileAt(5, y);
        }
        rookToTile->piece = rookTile->piece;
        rookTile->piece = nullptr;
    }
}

void ChessBoard::moveRook(const ChessTile *fromTile) {
    const int x = fromTile->getX();
    const int y = fromTile->getY();
    if (x == 0 && y == 0)
        whiteRookMoved.first = true;
    if (x == 7 && y == 0)
        whiteRookMoved.second = true;
    if (x == 7 && y == 7)
        blackRookMoved.second = true;
    if (x == 0 && y == 7)
        blackRookMoved.first = true;
}

ChessTile *ChessBoard::getTileAt(const std::string &pos) const {
    if (pos.size() != 2) {
        std::cout << "ChessBoard::getTileAt: Wrong input length." << std::endl;
        return nullptr;
    }
    const char xText = pos[0];
    if (xText != std::clamp(xText, 'a', 'h'))
        return nullptr;
    const int x = ChessTile::mapXtoInt.at(xText);
    const int y = pos[1] - '0'; // is a trick to convert number char to int
    return getTileAt(x, y);
}

ChessTile *ChessBoard::getTileAt(const int x, const int y) const {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight)
        return nullptr;
    return board[y * boardWidth + x];
}

void ChessBoard::mergePossVec(Pieces &possibleMoves, Pieces possibleMovesMerge) const {
    possibleMoves.insert(possibleMoves.end(), possibleMovesMerge.begin(), possibleMovesMerge.end());
}

void ChessBoard::pawnWon(ChessTile *pawnTile, const char pawnToPiece) const {
    updateBoard();
    std::cout << "Your pawn reached the end what should it become? (Q, R, B, N)" << std::endl;
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
    ChessPiece *newPawn = nullptr;
    switch(newPawnTyp) {
        case 'Q':
            newPawn = new ChessPiece(Queen, white);
            break;
        case 'R':
            newPawn = new ChessPiece(Rook, white);
            break;
        case 'B':
            newPawn = new ChessPiece(Bishop, white);
            break;
        case 'N':
            newPawn = new ChessPiece(Knight, white);
            break;
        default:
            std::cout << "you broke the game f u" << std::endl;
            break;
    }
    pawnTile->piece = newPawn;
}

GameState ChessBoard::afterMoveChecks(ChessTile *toTile, const char pawnToPiece) {
    doublePawnMoveAt = -1;
    if (toTile->piece->getType() == Pawn && (toTile->getY() == 0 || toTile->getY() == 7)) {
        pawnWon(toTile, pawnToPiece);
    }
    if (isDraw()) {
        std::cout << "this game is a draw!" << std::endl;
        return GameState::DRAW;
    }
    if (isKingCheckmate()) {
        std::cout << "you won the game!" << std::endl;
        return GameState::WON;
    }
    whitesTurn = !whitesTurn;
    return GameState::IN_PROGRESS;
}

std::optional<std::pair<ChessTile *, ChessTile *>> ChessBoard::getMoveTilesFromInput(const std::string &input) const {
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
    if (fromTile->piece == nullptr || fromTile->piece->isWhite() != whitesTurn) {
        std::cout << "ChessBoard::getMoveTilesFromInput: trying to move a piece from the opponent or no piece" << std::endl;
        return std::nullopt;
    }
    return std::make_optional(std::make_pair(fromTile, toTile));
}
