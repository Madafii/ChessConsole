#pragma once

#include "ChessPieceData.h"
#include "ChessTile.h"
#include "Renderer.h"
#include "VertexBufferDynamic.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>

using namespace ChessPieceData;

class ChessPieceSelectedGLDraw {
  public:
    explicit ChessPieceSelectedGLDraw(std::shared_ptr<SharedPieceRenderData> renderData);

    void onRender() const;
    void onUpdate();

    void setData(glm::vec2 pos, float layer, ChessTile *piece) {
        _instance = {pos, layer};
        _piece = piece;
    }
    void setUnselected() {
        _instance = std::nullopt;
        _piece = nullptr;
    }

    ChessTile *getPiece() const { return _piece; };
    glm::vec2 getPosition() const {
        assert(_instance);
        return _instance->position;
    }
    float getLayer() const {
        assert(_instance);
        return _instance->layer;
    }

    void updatePos(glm::vec2 pos) { _instance->position = pos; }

    bool hasInstance() const { return _instance.has_value(); };

  private:
    ChessTile *_piece;
    std::shared_ptr<SharedPieceRenderData> _renderData;
    std::unique_ptr<VertexBufferDynamic> _vertexBufferDynamic;
    std::unique_ptr<Renderer> _renderer = std::make_unique<Renderer>();
    std::unique_ptr<Shader> _shader = std::make_unique<Shader>("application/res/shaders/Selected.shader");
    std::optional<InstanceData> _instance;
};
