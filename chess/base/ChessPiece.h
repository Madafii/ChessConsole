#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <string>

enum ChessPieceType { King, Queen, Rook, Knight, Bishop, Pawn, None };

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
    const ChessPieceType _type;
    const bool _white;
    std::string _long;
    char _short;
};

#endif // CHESSPIECE_H
