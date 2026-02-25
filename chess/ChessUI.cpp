#include "ChessUI.h"

ChessUI::ChessUI(const PlayerFactory &playerWhite, const PlayerFactory &playerBlack) : _chessInterface() {
    _playerWhite = playerWhite(_chessInterface);
    _playerBlack = playerBlack(_chessInterface);
}
