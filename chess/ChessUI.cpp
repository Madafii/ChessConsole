#include "ChessUI.h"
#include "ChessPlayerConsoleHuman.h"

ChessUI::ChessUI(PlayerFactory playerWhite, PlayerFactory playerBlack) : _chessInterface(){
    _playerWhite = playerWhite(_chessInterface);
    _playerBlack = playerBlack(_chessInterface);
}
