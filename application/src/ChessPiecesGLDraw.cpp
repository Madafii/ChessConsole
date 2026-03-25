#include "ChessPiecesGLDraw.h"
#include "ChessGL.h"
#include "Renderer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>

ChessPiecesGLDraw::ChessPiecesGLDraw() : _translation(0, 0, 0) {
    float squareVertices[] = {
        0.0f,   0.0f,   0.0f, 0.0f, // 0
        0.125f, 0.0f,   1.0f, 0.0f, // 1
        0.125f, 0.125f, 1.0f, 1.0f, // 1
        0.0f,   0.125f, 0.0f, 1.0f  // 1
    };

    uint indicies[] = {
        0, 1, 2, // 0
        2, 3, 0  // 1
    };

    _instances = {{{0.0f, 0.0f}, 1.0f}, {{0.875f, 0.875f}, 0.0f}};

    _vertexArray = std::make_unique<VertexArray>();
    _vertexBuffer = std::make_unique<VertexBuffer>(squareVertices, sizeof(squareVertices));
    _indexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * sizeof(indicies));

    VertexBufferLayout layout;
    layout.Push<float>(2); // position
    layout.Push<float>(2); // uv

    _vertexArray->AddBuffer(*_vertexBuffer, layout);

    _vertexBufferDynamic = std::make_unique<VertexBufferDynamic>(_instances.data(), _instances.size() * sizeof(InstanceData));

    GLCall(glEnableVertexAttribArray(2));
    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void *)nullptr));
    GLCall(glVertexAttribDivisor(2, 1));

    GLCall(glEnableVertexAttribArray(3));
    GLCall(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void *)(sizeof(glm::vec2))));
    GLCall(glVertexAttribDivisor(3, 1));

    _shader = std::make_unique<Shader>("application/res/shaders/Piece.shader");
    _shader->Bind();
    _textureSet = std::make_unique<TextureSet>("application/res/textures/piecesSet.png", 12, 2);
    _shader->SetUniform1i("u_Texture", 0);

    _proj = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
    _view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void ChessPiecesGLDraw::OnRender() {
    Renderer renderer;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), _translation);
    glm::mat4 mvp = _proj * _view * model;

    _textureSet->Bind();
    _shader->Bind();
    _shader->SetUniform1i("u_Texture", 0);
    _shader->SetUniformMat4f("u_MVP", mvp);
    _vertexArray->Bind();
    _indexBuffer->Bind();
    GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, _instances.size()));
}

void ChessPiecesGLDraw::OnUpdate() {
    _vertexBufferDynamic->update(_instances.data(), _instances.size() * sizeof(InstanceData));
}
