#pragma once

#include "ChessBoard.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include <glm/ext/vector_float3.hpp>
#include <memory>

class ChessBoardGLDraw {
  public:
    explicit ChessBoardGLDraw();

    void OnRender() const;

    static constexpr float tileHeight = 1.0f / boardHeight;
    static constexpr float tileWidth = 1.0f / boardWidth;

    static constexpr size_t boardVertexBufferSize = (boardWidth + 1) * (boardHeight + 1) * 2;
    static constexpr size_t boardIndicesSize = boardSize * 2 * 3;

  private:
    std::unique_ptr<VertexArray> _vertexArray;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::unique_ptr<Shader> _shader;
    std::unique_ptr<Renderer> _renderer;
    glm::vec3 _translation;
    glm::mat4 _proj;
    glm::mat4 _view;

    static constexpr std::array<float, boardVertexBufferSize> getBoardPositions();
    static constexpr std::array<uint, boardIndicesSize> getBoardIndices();
};

constexpr std::array<float, ChessBoardGLDraw::boardVertexBufferSize> ChessBoardGLDraw::getBoardPositions() {
    std::array<float, ChessBoardGLDraw::boardVertexBufferSize> boardPositions;

    uint positionIndex = 0;
    for (size_t h = 0; h < boardHeight + 1; ++h) {
        for (size_t w = 0; w < boardWidth + 1; ++w) {
            boardPositions[positionIndex++] = tileWidth * w;
            boardPositions[positionIndex++] = tileHeight * h;
        }
    }
    return boardPositions;
}

constexpr std::array<uint, ChessBoardGLDraw::boardIndicesSize> ChessBoardGLDraw::getBoardIndices() {
    std::array<uint, ChessBoardGLDraw::boardIndicesSize> indices;
    int idx = 0;
    for (size_t y = 0; y < boardHeight; ++y) {
        for (size_t x = 0; x < boardWidth; ++x) {
            uint32_t topLeft = y * (boardWidth + 1) + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (y + 1) * (boardWidth + 1) + x;
            uint32_t bottomRight = bottomLeft + 1;

            // Triangle 1
            indices[idx++] = bottomLeft;
            indices[idx++] = bottomRight;
            indices[idx++] = topRight;

            // Triangle 2
            indices[idx++] = topRight;
            indices[idx++] = topLeft;
            indices[idx++] = bottomLeft;
        }
    }
    return indices;
}
