#include "ChessPieceSelectedGLDraw.h"

#include "ChessPiecesGLDraw.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>

ChessPieceSelectedGLDraw::ChessPieceSelectedGLDraw() {
    _vertexArray = std::make_unique<VertexArray>();

    _indexBuffer = std::make_unique<IndexBuffer>(ChessPiecesGLDraw::pieceIndicies, sizeof(ChessPiecesGLDraw::pieceIndicies));

    _vertexBuffer = std::make_unique<VertexBuffer>(ChessPiecesGLDraw::squareVertices, sizeof(ChessPiecesGLDraw::squareVertices));

    VertexBufferLayout layout;
    layout.Push<float>(2); // position single piece
    layout.Push<float>(2); // uv

    _vertexArray->AddBuffer(*_vertexBuffer, layout);

    _vertexBufferDynamic = std::make_unique<VertexBufferDynamic>(nullptr, sizeof(InstanceData));

    VertexBufferLayout layoutDynamic;
    layoutDynamic.Push<float>(2); // positon board
    layoutDynamic.Push<float>(1); // layer

    _vertexArray->AddBuffer(*_vertexBufferDynamic, layoutDynamic);

    // _shader = std::make_unique<Shader>("application/res/shaders/Piece.shader");
    // _shader->Bind();
    // _shader->SetUniform1i("u_Texture", 0);
    //
    // _proj = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
    // _view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void ChessPieceSelectedGLDraw::onRender() {
    if (!_instance) return;

    // Renderer renderer;
    // glm::mat4 model = glm::translate(glm::mat4(1.0f), _translation);
    // glm::mat4 mvp = _proj * _view * model;

    // _shader->Bind();
    // _shader->SetUniform1i("u_Texture", 0);
    // _shader->SetUniformMat4f("u_MVP", mvp);
    _vertexArray->Bind();
    _indexBuffer->Bind();
    GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 1));
}

void ChessPieceSelectedGLDraw::onUpdate() {
    if (!_instance) return;
    _vertexBufferDynamic->update(&_instance.value(), sizeof(InstanceData));
}
