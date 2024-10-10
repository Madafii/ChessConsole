//
// Created by fpittermann on 10/5/24.
//

#ifndef CHESSPIECE_H
#define CHESSPIECE_H
#include <string>

enum ChessPieceType {
    King,
    Queen,
    Rook,
    Knight,
    Bishop,
    Pawn
};

class ChessPiece {
public:
    explicit ChessPiece(ChessPieceType type, bool white);
    ~ChessPiece();

    ChessPieceType getType() const;
    bool isWhite() const;

    std::string getFullName() const;
    char getShortName() const;
private:
    ChessPieceType _type;
    bool _white;
    std::string _long;
    char _short;
};



#endif //CHESSPIECE_H
