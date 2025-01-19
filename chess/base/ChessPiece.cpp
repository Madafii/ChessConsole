#include "ChessPiece.h"
#include <iostream>

// TODO: could make this constexpr?

ChessPiece::ChessPiece(const ChessPieceType type, const bool white) : _type(type), _white(white) {
    switch (_type) {
        case King:
            _short = 'K';
            _long = "King";
            break;
        case Queen:
            _short = 'Q';
            _long = "Queen";
            break;
        case Bishop:
            _short = 'B';
            _long = "Bishop";
            break;
        case Knight:
            _short = 'N';
            _long = "Knight";
            break;
        case Rook:
            _short = 'R';
            _long = "Rook";
            break;
        case Pawn:
            _short = 'P';
            _long = "Pawn";
            break;
        default:
            _short = '?';
            _long = "?";
            break;
    }
}

ChessPiece::~ChessPiece() = default;

ChessPieceType ChessPiece::getType() const { return _type; }

bool ChessPiece::isWhite() const { return _white; }

std::string ChessPiece::getFullName() const { return _long; }

char ChessPiece::getShortName() const { return _short; }

inline ChessPieceType ChessPiece::getTypeFromShort(const char &shortName) {
    switch (shortName) {
        case 'K':
            return King;
        case 'Q':
            return Queen;
        case 'B':
            return Bishop;
        case 'N':
            return Knight;
        case 'R':
            return Rook;
        case 'P':
            return Pawn;
        default:
            std::cout << "that is not a ChessPieceType" << std::endl;
            return None;
    }
}
