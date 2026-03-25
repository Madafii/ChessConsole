#include "ChessBoardGLDraw.h"
#include "Renderer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>

ChessBoardGLDraw::ChessBoardGLDraw() : _translation(0, 0, 0) {
    constexpr auto boardPositions = getBoardPositions();
    constexpr auto boardIndicies = getBoardIndicies();

    _vertexArray = std::make_unique<VertexArray>();
    _vertexBuffer = std::make_unique<VertexBuffer>(boardPositions.data(), ChessBoardGLDraw::boardVertexBufferSize * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);

    _vertexArray->AddBuffer(*_vertexBuffer, layout);
    _indexBuffer = std::make_unique<IndexBuffer>(boardIndicies.data(), ChessBoardGLDraw::boardIndicesSize);

    _shader = std::make_unique<Shader>("application/res/shaders/Board.shader");
    _shader->Bind();
    _shader->SetUniform1f("u_TileWidth", (float)(tileWidth));
    _shader->SetUniform1f("u_TileHeight", (float)(tileHeight));

    _proj = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
    _view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void ChessBoardGLDraw::OnRender() {
    Renderer renderer;
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), _translation);
        glm::mat4 mvp = _proj * _view * model;
        _shader->Bind();
        _shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*_vertexArray, *_indexBuffer, *_shader);
    }
}
