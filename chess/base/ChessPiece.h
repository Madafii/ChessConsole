#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <map>
#include <string>

enum class ChessPieceType { KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN, NONE };

class ChessPiece {
  private:
    // default is a empty tile
    ChessPieceType _type = ChessPieceType::NONE;
    bool _white = true;
    std::string _long = "_";
    char _short = '_';

    static inline std::map<char, ChessPieceType> _shortToEnum = {
        {'K', ChessPieceType::KING}, {'Q', ChessPieceType::QUEEN}, {'N', ChessPieceType::KNIGHT}, {'B', ChessPieceType::BISHOP},
        {'R', ChessPieceType::ROOK}, {'P', ChessPieceType::PAWN},  {'_', ChessPieceType::NONE}};

  public:
    ChessPiece() = default;
    ChessPiece(ChessPieceType type, bool white);

    ChessPieceType getType() const { return _type; };
    bool isWhite() const { return _white; };
    std::string_view getLongName() const { return _long; }
    char getShortName() const { return _short; }

    static ChessPieceType getTypeFromShort(const char &shortName);
};

#endif // CHESSPIECE_H
