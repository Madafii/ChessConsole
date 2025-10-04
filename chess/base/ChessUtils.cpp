#include "ChessUtils.h"
#include "ChessMoveLogic.h"

std::string ChessUtils::convertPGNToMyInput(std::string input, ChessMoveLogic &chessLogic, const bool &white) {
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
    std::string extra;
    switch (input[0]) {
        case 'K':
            typeToMove = ChessPieceType::KING;
            break;
        case 'Q':
            typeToMove = ChessPieceType::QUEEN;
            break;
        case 'R':
            typeToMove = ChessPieceType::ROOK;
            break;
        case 'B':
            typeToMove = ChessPieceType::BISHOP;
            break;
        case 'N':
            typeToMove = ChessPieceType::KNIGHT;
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
            typeToMove = ChessPieceType::PAWN;
            break;
    }
    if (Leftovers.size() >= 2) extra = input.substr(1, 1);
    if (Leftovers.size() >= 3) extra = input.substr(1, 2);
    if (typeToMove == ChessPieceType::PAWN) {
        if (Leftovers.size() >= 1) extra = input.substr(0, 1);
    }
    // find the piece To move
    const PieceTiles pieces = chessLogic.getChessBoard().getAllPiecesFor(white, typeToMove);
    const ChessTile *foundFromTile = nullptr;
    std::vector<std::pair<const ChessTile *, const ChessTile *>> foundMoves;
    for (const auto *piece : pieces) {
        PieceTiles possibleMoves = chessLogic.getPossibleMoves(*piece);
        auto it = std::ranges::find_if(possibleMoves, [&](const ChessTile *tile) {
            if (tile->getX() == ChessTile::mapXtoInt.at(moveTo[0]) && tile->getY() + 1 == moveTo[1] - '0') {
                return true;
            }
            return false;
        });
        if (it == possibleMoves.end()) continue;
        foundMoves.emplace_back(piece, (*it));
    }
    // only one possibility
    if (foundMoves.size() == 1) {
        foundFromTile = foundMoves[0].first;
    } else {
        for (const auto &[fromTile, toTile] : foundMoves) {
            // check if not possible by being checked
            PieceTiles filterPieces{toTile};
            chessLogic.filterPossibleMovesForChecks(*fromTile, filterPieces);
            if (filterPieces.size() == 0) continue;
            foundFromTile = fromTile;
            // is like a move hint if there are multiple same pieces that have the to move as a possibility
            std::string tmpMoveTo = fromTile->getMove();
            if (extra.size() == 1) {
                if (std::ranges::find(tmpMoveTo, extra[0]) != tmpMoveTo.end()) {
                    foundFromTile = fromTile;
                    break;
                }
            } else if (extra.size() == 2) {
                if (std::ranges::find(tmpMoveTo, extra[0]) != tmpMoveTo.end() &&
                    std::ranges::find(tmpMoveTo, extra[1]) != tmpMoveTo.end()) {
                    foundFromTile = fromTile;
                    break;
                }
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
