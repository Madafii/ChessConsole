//
// Created by finnp on 10/26/24.
//

#include "ChessUtils.h"
#include "ChessMoveLogic.h"

std::string ChessUtils::convertPGNToMyInput(std::string input, ChessBoard &board, const bool &white) {
    // ignore sume values in the input
    // Remove '+' characters
    std::erase(input, '+');
    // Remove '#' characters
    std::erase(input, '#');
    // Remove 'x' characters
    std::erase(input, 'x');
    // Remove analzyer characters
    std::erase(input, '?');
    std::erase(input, '!');
    // extract '=' pawn to piece change
    char pawnChange = 'L'; // L for no change
    auto it = std::ranges::find(input, '=');
    if (it != input.end()) {
        ++it;
        pawnChange = *it;
        input = input.substr(0, input.size() - 2);
    }
    ChessPieceType typeToMove;
    const std::string moveTo = input.substr(input.size() - 2, 2);
    const std::string Leftovers = input.substr(0, input.size() - 2);
    std::string extra = "";
    switch (input[0]) {
    case 'K':
        typeToMove = King;
        break;
    case 'Q':
        typeToMove = Queen;
        break;
    case 'R':
        typeToMove = Rook;
        break;
    case 'B':
        typeToMove = Bishop;
        break;
    case 'N':
        typeToMove = Knight;
        break;
    case 'O':
        if (input.size() == 3) {
            if (white) {
                return "e1:g1";
            }
            return "e8:g8";
        }
        if (white) {
            return "e1:c1";
        }
        return "e8:c8";
    default: // pawns
        typeToMove = Pawn;
        break;
    }
    if (Leftovers.size() >= 2)
        extra = input.substr(1, 1);
    if (Leftovers.size() >= 3)
        extra = input.substr(1, 2);
    if (typeToMove == Pawn) {
        if (Leftovers.size() >= 1)
            extra = input.substr(0, 1);
    }
    // find the piece To move
    // Pieces pieces = board.getAllPossibleMovesPiece(white, typeToMove);
    // while (it != pieces.end()) {
    const Pieces pieces = board.getAllPiecesFor(white, typeToMove);
    const ChessTile *foundFromTile = nullptr;
    // Pieces::iterator it = pieces.begin();
    for (const auto &piece : pieces) {
        Pieces possibleMoves = ChessMoveLogic::getPossibleMoves(board, piece);
        ChessMoveLogic::filterPossibleMovesForChecks(board, piece, possibleMoves);
        auto it = std::ranges::find_if(possibleMoves, [&](const ChessTile *tile) {
            if (piece->piece->getType() == typeToMove && tile->getX() == ChessTile::mapXtoInt.at(moveTo[0]) &&
                tile->getY() + 1 == moveTo[1] - '0') {
                return true;
            }
            return false;
        });
        if (it == possibleMoves.end())
            continue;
        foundFromTile = piece;
        if (extra.size() == 0)
            break;
        // is like a move hint if there are multiple same pieces that have the to move as a possibility
        std::string tmpMoveTo = piece->getMove();
        if (extra.size() == 1) {
            if (std::ranges::find(tmpMoveTo, extra[0]) != tmpMoveTo.end()) {
                foundFromTile = piece;
                break;
            }
        } else if (extra.size() == 2) {
            if (std::ranges::find(tmpMoveTo, extra[0]) != tmpMoveTo.end() && std::ranges::find(tmpMoveTo, extra[1]) != tmpMoveTo.end()) {
                foundFromTile = piece;
                break;
            }
        }
    }
    std::string ret = foundFromTile->getMove() + ":" + std::string(moveTo);
    // pawn piece got changed
    if (pawnChange != 'L') {
        ret.append("=" + std::string(1, pawnChange));
    }
    return ret;
}
