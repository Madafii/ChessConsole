#pragma once

#include "ChessBoard.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include <glm/ext/vector_float3.hpp>
#include <memory>

class ChessBoardGLDraw {
  public:
    explicit ChessBoardGLDraw();

    void create();
    void OnRender();

    static constexpr float tileHeight = 1.0f / boardHeight;
    static constexpr float tileWidth = 1.0f / boardWidth;

    static constexpr size_t boardVertexBufferSize = (boardWidth + 1) * (boardHeight + 1) * 2;
    static constexpr size_t boardIndicesSize = boardSize * 2 * 3;

  private:
    std::unique_ptr<VertexArray> _vertexArray;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::unique_ptr<Shader> _shader;
    glm::vec3 _translation;
    glm::mat4 _proj;
    glm::mat4 _view;

    static constexpr std::array<float, boardVertexBufferSize> getBoardPositions();
    static constexpr std::array<uint, boardIndicesSize> getBoardIndicies();
};

constexpr std::array<float, ChessBoardGLDraw::boardVertexBufferSize> ChessBoardGLDraw::getBoardPositions() {
    std::array<float, ChessBoardGLDraw::boardVertexBufferSize> boardPositions;

    uint positionIndex = 0;
    for (int h = 0; h < boardHeight + 1; ++h) {
        for (int w = 0; w < boardWidth + 1; ++w) {
            boardPositions[positionIndex] = tileHeight * h;
            positionIndex++;
            boardPositions[positionIndex] = tileWidth * w;
            positionIndex++;
        }
    }
    return boardPositions;
}

constexpr std::array<uint, ChessBoardGLDraw::boardIndicesSize> ChessBoardGLDraw::getBoardIndicies() {
    std::array<uint, ChessBoardGLDraw::boardIndicesSize> boardIndicies;
    uint rightFirstIndice = boardWidth + 1;
    uint rightSecondIndice = boardWidth + 2;
    uint rightThirdIndice = 1;
    uint leftFirstIndice = 1;
    uint leftSecondIndice = 0;
    uint leftThirdIndice = boardWidth + 1;

    for (size_t i = 0; i < ChessBoardGLDraw::boardIndicesSize; i += 6) {
        boardIndicies[i] = rightFirstIndice;
        boardIndicies[i + 1] = rightSecondIndice;
        boardIndicies[i + 2] = rightThirdIndice;
        boardIndicies[i + 3] = leftFirstIndice;
        boardIndicies[i + 4] = leftSecondIndice;
        boardIndicies[i + 5] = leftThirdIndice;
        if (rightSecondIndice % (boardWidth + 1) != boardWidth) {
            rightFirstIndice++;
            rightSecondIndice++;
            rightThirdIndice++;
            leftFirstIndice++;
            leftSecondIndice++;
            leftThirdIndice++;
        } else {
            rightFirstIndice += 2;
            rightSecondIndice += 2;
            rightThirdIndice += 2;
            leftFirstIndice += 2;
            leftSecondIndice += 2;
            leftThirdIndice += 2;
        }
    }
    return boardIndicies;
}
