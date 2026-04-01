#pragma once

#include "ChessTile.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferDynamic.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>

class ChessPieceSelectedGLDraw {
  public:
    struct InstanceData {
        glm::vec2 position;
        float layer;
    };

    explicit ChessPieceSelectedGLDraw();

    void onRender();
    void onUpdate();

    void setData(glm::vec2 pos, float layer, ChessTile *piece) {
        _instance = {pos, layer};
        _piece = piece;
    }
    void setUnselected() { _instance = std::nullopt; }

    ChessTile *getPiece() { return _piece; };
    glm::vec2 getPosition() { return _instance->position; }
    float getLayer() { return _instance->layer; }

    void updatePos(glm::vec2 pos) { _instance->position = pos; }

    bool hasInstance() { return _instance.has_value(); };

  private:
    ChessTile *_piece;
    std::optional<InstanceData> _instance;
    std::unique_ptr<VertexArray> _vertexArray;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<VertexBufferDynamic> _vertexBufferDynamic;
    std::unique_ptr<IndexBuffer> _indexBuffer;
};
