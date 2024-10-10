//
// Created by fpittermann on 10/5/24.
//

#include "ChessPiece.h"

ChessPiece::ChessPiece(const ChessPieceType type, const bool white)
: _type(type), _white(white) {
    switch (_type) {
        case King: {
            _short = 'K';
            _long = "King";
            break;
        }
        case Queen: {
            _short = 'Q';
            _long = "Queen";
            break;
        }
        case Bishop: {
            _short = 'B';
            _long = "Bishop";
            break;
        }
        case Knight: {
            _short = 'N';
            _long = "Knight";
            break;
        }
        case Rook: {
            _short = 'R';
            _long = "Rook";
            break;
        }
        case Pawn: {
            _short = 'P';
            _long = "Pawn";
            break;
        }
        default: {
            _short = '?';
            _long = "?";
            break;
        }
    }
}

ChessPiece::~ChessPiece() {}

ChessPieceType ChessPiece::getType() const {
    return _type;
}

bool ChessPiece::isWhite() const {
    return _white;
}

std::string ChessPiece::getFullName() const {
    return std::string("");
}

char ChessPiece::getShortName() const {
    return _short;
}