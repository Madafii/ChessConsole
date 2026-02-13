#include "ChessUI.h"
#include "ChessPlayerConsoleHuman.h"

ChessUI::ChessUI(PlayerType playerWhite, PlayerType playerBlack) {
    switch(playerWhite) {
        case PlayerType::ConsoleHuman:
        _playerWhite = std::make_unique<ChessPlayerConsoleHuman>(_chessInterface);
    }

    switch(playerBlack) {
        case PlayerType::ConsoleHuman:
        _playerBlack = std::make_unique<ChessPlayerConsoleHuman>(_chessInterface);
    }

}
