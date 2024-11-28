//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSPIECE_H
#define CHESSPIECE_H
#include <iostream>
#include <string>

enum ChessPieceType {
    King,
    Queen,
    Rook,
    Knight,
    Bishop,
    Pawn,
    None
};

class ChessPiece {
public:
    explicit ChessPiece(ChessPieceType type, bool white);
    ~ChessPiece();

    ChessPieceType getType() const;
    bool isWhite() const;

    std::string getFullName() const;
    char getShortName() const;

    static ChessPieceType getTypeFromShort(const char &shortName);
private:
    ChessPieceType _type;
    bool _white;
    std::string _long;
    char _short;
};

inline ChessPieceType ChessPiece::getTypeFromShort(const char &shortName) {
    switch (shortName) {
        case 'K': {
            return King;
        }
        case 'Q': {
            return Queen;
        }
        case 'B': {
            return Bishop;
        }
        case 'N': {
            return Knight;
        }
        case 'R': {
            return Rook;
        }
        case 'P': {
            return  Pawn;
        }
        default: {
            std::cout << "that is not a ChessPieceType" << std::endl;
            return None;
        }
    }
}

#endif //CHESSPIECE_H
