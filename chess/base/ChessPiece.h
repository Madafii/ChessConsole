#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <string>

enum class ChessPieceType { KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN, NONE };

class ChessPiece {
  public:
    explicit ChessPiece(ChessPieceType type, bool white);
    // ChessPiece(ChessPiece &&other) noexcept;
    ~ChessPiece() = default;

    [[nodiscard]] inline ChessPieceType getType() const { return _type; };
    [[nodiscard]] inline bool isWhite() const { return _white; };
    [[nodiscard]] inline std::string_view getLongName() const { return _long; }
    [[nodiscard]] inline char getShortName() const { return _short; }

    static ChessPieceType getTypeFromShort(const char &shortName);

  private:
    ChessPieceType _type;
    bool _white;
    std::string _long;
    char _short;
};

#endif // CHESSPIECE_H
