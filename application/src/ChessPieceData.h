#include "ChessBoard.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>

namespace ChessPieceData {

struct InstanceData {
    glm::vec2 position;
    float layer;
};

static constexpr float _tileHeight = 1.0f / boardHeight;
static constexpr float _tileWidth = 1.0f / boardWidth;
static constexpr float _pieceHeight = _tileHeight;
static constexpr float _pieceWidth = _tileWidth;
static constexpr size_t _pieceSetSize = 12;
static constexpr size_t _pieceSetRows = 2;

static constexpr std::array<float, 32> squareVertices = {
    0.0f,       0.0f,        0.0f, 0.0f, // 0
    _tileWidth, 0.0f,        1.0f, 0.0f, // 1
    _tileWidth, _tileHeight, 1.0f, 1.0f, // 2
    0.0f,       _tileHeight, 0.0f, 1.0f  // 3
};

static constexpr std::array<uint, 6> pieceIndicies = {
    0, 1, 2, // 0
    2, 3, 0  // 1
};

struct SharedPieceRenderData {
    SharedPieceRenderData() : VAO{}, VBO{squareVertices.data(), 32 * sizeof(float)}, IBO{pieceIndicies.data(), pieceIndicies.size()} {
        VertexBufferLayout layout;
        layout.Push<float>(2); // position single piece
        layout.Push<float>(2); // uv
                               //
        VAO.AddBuffer(VBO, layout);
    }
    VertexArray VAO;
    VertexBuffer VBO;
    IndexBuffer IBO;

    glm::vec3 translation;
    glm::mat4 proj = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 mvp = proj * view * model;
};

} // namespace ChessPieceData
