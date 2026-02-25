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
        case NONE:
            _short = '_';
            _long = "_";
            break;
        default:
            _short = '?';
            _long = "?";
            break;
    }
}

// compare with upper case short name
ChessPieceType ChessPiece::getTypeFromShort(const char &shortName) {
    if (_shortToEnum.contains(shortName)) {
        return _shortToEnum[shortName];
    }
    // default if no other valid piece type was passed
    return ChessPieceType::QUEEN;
}
