#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <string>

enum ChessPieceType { King, Queen, Rook, Knight, Bishop, Pawn, None };

class ChessPiece {
  public:
    explicit ChessPiece(ChessPieceType type, bool white);
    ~ChessPiece();

    [[nodiscard]] ChessPieceType getType() const;
    [[nodiscard]] bool isWhite() const;

    [[nodiscard]] std::string getFullName() const;
    [[nodiscard]] char getShortName() const;

    static ChessPieceType getTypeFromShort(const char &shortName);

  private:
    const ChessPieceType _type;
    const bool _white;
    std::string _long;
    char _short;
};

#endif // CHESSPIECE_H
