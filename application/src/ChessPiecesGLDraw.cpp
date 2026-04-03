#include "ChessPiecesGLDraw.h"
#include "ChessBoard.h"
#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <optional>

ChessPiecesGLDraw::ChessPiecesGLDraw(ChessInterface &interface) : _chessInterface(interface), _selected(_renderData) {
    updateInstanceData();
    _vertexBufferDynamic = std::make_unique<VertexBufferDynamic>(_instances.data(), _instances.size() * sizeof(InstanceData));

    VertexBufferLayout layoutDynamic;
    layoutDynamic.Push<float>(2); // positon board
    layoutDynamic.Push<float>(1); // layer

    _renderData->VAO.AddBuffer(*_vertexBufferDynamic, layoutDynamic);
}

void ChessPiecesGLDraw::onRender() {
    _textureSet->Bind();
    _shader->Bind();
    _shader->SetUniform1i("u_Texture", 0);
    _shader->SetUniformMat4f("u_MVP", _renderData->mvp);
    _renderData->VAO.Bind();
    _renderData->IBO.Bind();
    _renderer->DrawDynamic(_renderData->VAO, _renderData->IBO, *_shader, _instances.size());

    _selected.onRender();
}

void ChessPiecesGLDraw::onUpdate(double xPos, double yPos) {
    if (_selected.hasInstance()) {
        _selected.updatePos({xPos - _tileWidth / 2, 1.0 - yPos - _tileHeight / 2});
    }

    _selected.onUpdate();
}

void ChessPiecesGLDraw::onClick(double xPos, double yPos) {
    unselect();

    // select instance of InstanceData
    auto selectedData = getInstance(xPos, yPos);
    if (!selectedData) return;

    // select board tile
    const auto fromTile = boardToTile(*screenToBoard(xPos, yPos));

    // check for possible moves
    auto possMoves = _chessInterface.getPossibleMovesFromTile(fromTile->getPos());
    if (!possMoves || possMoves.value().empty()) return;

    // select piece and cache possible moves
    _selected.setData(selectedData.value()->position, selectedData.value()->layer, fromTile);
    _selected.onUpdate();
    _possibleMoves = std::move(*possMoves);

    eraseInstance(**selectedData);
    updateInstanceBuffer();
}

void ChessPiecesGLDraw::onDrop(double xPos, double yPos) {
    auto boardPos = screenToBoard(xPos, yPos);
    if (!boardPos) return;

    auto toTile = boardToTile(*boardPos);

    // check if valid move then make it
    for (const auto &tile : _possibleMoves) {
        if (toTile == tile) {
            _chessInterface.handleMoveInputNoChecks(*_selected.getPiece(), *toTile);
            break;
        }
    }

    unselect();

    updateInstanceData();
    updateInstanceBuffer();

    _selected.onUpdate();

    onUpdate(xPos, yPos);
}

void ChessPiecesGLDraw::updateInstanceData() {
    _instances.clear();
    for (const auto &tile : _chessInterface.getChessBoard().getBoard()) {
        if (!tile.hasPiece()) continue;

        glm::vec2 pos{tile.getX() * _tileWidth, tile.getY() * _tileHeight};
        float layer = getLayer(tile.getPieceType(), tile.hasWhitePiece());
        _instances.emplace_back(pos, layer);
    }
}

std::optional<std::pair<int, int>> ChessPiecesGLDraw::screenToBoard(double xPos, double yPos) {
    int x = static_cast<int>(xPos * boardWidth);
    int y = boardHeight - 1 - static_cast<int>(yPos * boardHeight);

    if (!ChessBoard::validTilePos(x, y)) return std::nullopt;
    return {{x, y}};
}

void ChessPiecesGLDraw::unselect() {
    _selected.setUnselected();
    _possibleMoves.clear();
}

void ChessPiecesGLDraw::eraseInstance(const InstanceData &data) {
    auto found = std::ranges::find_if(_instances, [&](const InstanceData &instanceData) {
        return instanceData.position == data.position && instanceData.layer == data.layer;
    });
    if (found == _instances.end()) return;
    _instances.erase(found);
}

void ChessPiecesGLDraw::updateInstanceBuffer() {
    _vertexBufferDynamic->update(_instances.data(), _instances.size() * sizeof(InstanceData));
}

bool ChessPiecesGLDraw::inTileArea(glm::vec2 tilePos, double xPos, double yPos) {
    return ((xPos <= tilePos.x + _tileWidth && xPos >= tilePos.x) && 1.0 - yPos <= tilePos.y + _tileHeight && 1.0 - yPos >= tilePos.y);
}

std::optional<ChessPieceData::InstanceData *> ChessPiecesGLDraw::getInstance(double xPos, double yPos) {
    for (auto &instance : _instances) {
        if (inTileArea(instance.position, xPos, yPos)) {
            return &instance;
        }
    }
    return std::nullopt;
}
