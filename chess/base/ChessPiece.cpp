#include "ChessPiece.h"
#include <iostream>

ChessPiece::ChessPiece(const ChessPieceType type, const bool white) : _type(type), _white(white) {
    using enum ChessPieceType;
    switch (_type) {
        case KING:
            _short = 'K';
            _long = "King";
            break;
        case QUEEN:
            _short = 'Q';
            _long = "Queen";
            break;
        case BISHOP:
            _short = 'B';
            _long = "Bishop";
            break;
        case KNIGHT:
            _short = 'N';
            _long = "Knight";
            break;
        case ROOK:
            _short = 'R';
            _long = "Rook";
            break;
        case PAWN:
            _short = 'P';
            _long = "Pawn";
            break;
        default:
            _short = '?';
            _long = "?";
            break;
    }
}

// ChessPiece::ChessPiece(ChessPiece &&other) noexcept {
//     this->_type = other._type;
//     this->_long = other._long;
//     this->_short = other._short;
//     this->_white = other._white;
// }

ChessPieceType ChessPiece::getTypeFromShort(const char &shortName) {
    using enum ChessPieceType;
    switch (shortName) {
        case 'K':
            return KING;
        case 'Q':
            return QUEEN;
        case 'B':
            return BISHOP;
        case 'N':
            return KNIGHT;
        case 'R':
            return ROOK;
        case 'P':
            return PAWN;
        default:
            std::cout << "that is not a ChessPieceType" << std::endl;
            return NONE;
    }
}
