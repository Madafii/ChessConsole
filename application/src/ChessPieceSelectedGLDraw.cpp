#include "ChessPieceSelectedGLDraw.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <utility>

ChessPieceSelectedGLDraw::ChessPieceSelectedGLDraw(std::shared_ptr<SharedPieceRenderData> renderData) : _renderData(std::move(renderData)) {
    _instances.reserve(boardSize);
    _vertexBufferDynamic = std::make_unique<VertexBufferDynamic>(nullptr, boardSize * sizeof(InstanceData));

    VertexBufferLayout layoutDynamic;
    layoutDynamic.Push<float>(2); // positon board
    layoutDynamic.Push<float>(1); // layer

    _renderData->VAO.AddBuffer(*_vertexBufferDynamic, layoutDynamic);
}

void ChessPieceSelectedGLDraw::setData(const InstanceData &pieceData, ChessTile *piece, const PieceTiles &legalMoves) {
    // The selected-piece shader uses negative texture layers for move markers.
    constexpr float moveMarker = -1.0f;
    constexpr float captureMarker = -2.0f;
    _piece = piece;
    _instances.clear();
    for (const auto *tile : legalMoves) {
        const bool diagonalPawnMove = piece->hasPiece(ChessPieceType::PAWN) && piece->getX() != tile->getX();
        const float marker = tile->hasPiece() || diagonalPawnMove ? captureMarker : moveMarker;
        _instances.emplace_back(glm::vec2{tile->getX() * _tileWidth, tile->getY() * _tileHeight}, marker);
    }
    _instances.push_back(pieceData);
    onUpdate();
}

void ChessPieceSelectedGLDraw::onRender() const {
    if (!hasInstance()) return;

    _shader->Bind();
    _shader->SetUniform1i("u_Texture", 0);
    _shader->SetUniformMat4f("u_MVP", _renderData->mvp);
    _renderData->VAO.Bind();
    _renderData->IBO.Bind();
    _renderer->DrawDynamic(_renderData->VAO, _renderData->IBO, *_shader, _instances.size());
}

void ChessPieceSelectedGLDraw::onUpdate() {
    if (!hasInstance()) return;
    _vertexBufferDynamic->update(_instances.data(), _instances.size() * sizeof(InstanceData));
}
