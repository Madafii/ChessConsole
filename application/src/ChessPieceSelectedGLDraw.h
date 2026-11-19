#pragma once

#include "ChessPieceData.h"
#include "ChessTile.h"
#include "Renderer.h"
#include "VertexBufferDynamic.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <vector>

using namespace ChessPieceData;

class ChessPieceSelectedGLDraw {
  public:
    explicit ChessPieceSelectedGLDraw(std::shared_ptr<SharedPieceRenderData> renderData);

    void onRender() const;
    void onUpdate();

    void setData(const InstanceData &pieceData, ChessTile *piece, const PieceTiles &legalMoves);
    void setUnselected() {
        _instances.clear();
        _piece = nullptr;
    }

    ChessTile *getPiece() const { return _piece; }
    void updatePos(glm::vec2 pos) { _instances.back().position = pos; }
    bool hasInstance() const { return _piece != nullptr; }

  private:
    ChessTile *_piece = nullptr;
    std::shared_ptr<SharedPieceRenderData> _renderData;
    std::unique_ptr<VertexBufferDynamic> _vertexBufferDynamic;
    std::unique_ptr<Renderer> _renderer = std::make_unique<Renderer>();
    std::unique_ptr<Shader> _shader = std::make_unique<Shader>("application/res/shaders/Selected.shader");
    std::vector<InstanceData> _instances;
};
