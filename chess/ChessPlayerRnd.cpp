#include "ChessPlayerRnd.h"
#include "ChessMoveLogic.h"
#include "ChessPiece.h"
#include "ChessTile.h"

#include <format>
#include <optional>
#include <random>

ChessPlayerRnd::ChessPlayerRnd(const ChessInterface &chessInterface) : ChessPlayer(chessInterface), _gen(std::random_device{}()) {}

ChessPlayerRnd::~ChessPlayerRnd() = default;

std::optional<std::string> ChessPlayerRnd::getNextMove() const {
    const PieceMoves allLegalMoves = _chessInterface.getChessMoveLogic().getAllLegalMoves();
    if (allLegalMoves.empty()) return std::nullopt;

    std::uniform_int_distribution<> distrPossMoves(0, allLegalMoves.size() - 1);
    const auto &[fromTile, toTile] = allLegalMoves.at(distrPossMoves(_gen));

    std::string moveInput = std::format("{}:{}", fromTile->getPos(), toTile->getPos());
    if (fromTile->hasPiece(ChessPieceType::PAWN) && (toTile->getY() == 7 || toTile->getY() == 0)) {
        moveInput += getRndPawnPromotion();
    }
    return moveInput;
}

std::string_view ChessPlayerRnd::getRndPawnPromotion() const {
    std::uniform_int_distribution<> distrPawnPromo(0, 3);
    return promo[distrPawnPromo(_gen)];
}
