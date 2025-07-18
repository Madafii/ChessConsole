#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <string>

enum class ChessPieceType { KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN, NONE };

class ChessPiece {
  public:
    ChessPiece() = default;
    ChessPiece(ChessPieceType type, bool white);
    // ChessPiece(const ChessPiece &other) = default;

    [[nodiscard]] ChessPieceType getType() const { return _type; };
    [[nodiscard]] bool isWhite() const { return _white; };
    [[nodiscard]] std::string_view getLongName() const { return _long; }
    [[nodiscard]] char getShortName() const { return _short; }

    static ChessPieceType getTypeFromShort(const char &shortName);

  private:
    // default is a empty tile
    ChessPieceType _type = ChessPieceType::NONE;
    bool _white = true;
    std::string _long = "_";
    char _short = '_';
};

#endif // CHESSPIECE_H
