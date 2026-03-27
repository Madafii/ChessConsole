#include "ChessPiecesGLDraw.h"
#include "ChessBoard.h"
#include "ChessPiece.h"
#include "Renderer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>

ChessPiecesGLDraw::ChessPiecesGLDraw(ChessBoard &board) : _chessBoard(board) {
    float squareVertices[] = {
        0.0f,        0.0f,         0.0f, 0.0f, // 0
        _pieceWidth, 0.0f,         1.0f, 0.0f, // 1
        _pieceWidth, _pieceHeight, 1.0f, 1.0f, // 2
        0.0f,        _pieceHeight, 0.0f, 1.0f  // 3
    };

    uint pieceIndicies[] = {
        0, 1, 2, // 0
        2, 3, 0  // 1
    };

    _vertexArray = std::make_unique<VertexArray>();

    _indexBuffer = std::make_unique<IndexBuffer>(pieceIndicies, sizeof(pieceIndicies));

    _vertexBuffer = std::make_unique<VertexBuffer>(squareVertices, sizeof(squareVertices));

    VertexBufferLayout layout;
    layout.Push<float>(2); // position single piece
    layout.Push<float>(2); // uv

    _vertexArray->AddBuffer(*_vertexBuffer, layout);

    updateInstanceData();
    _vertexBufferDynamic = std::make_unique<VertexBufferDynamic>(_instances.data(), _instances.size() * sizeof(InstanceData));

    VertexBufferLayout layoutDynamic;
    layoutDynamic.Push<float>(2); // positon board
    layoutDynamic.Push<float>(1); // layer

    _vertexArray->AddBuffer(*_vertexBufferDynamic, layoutDynamic);

    _shader = std::make_unique<Shader>("application/res/shaders/Piece.shader");
    _shader->Bind();
    _textureSet = std::make_unique<TextureSet>("application/res/textures/piecesSet.png", _pieceSetSize, _pieceSetRows);
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

void ChessPiecesGLDraw::OnUpdate() { _vertexBufferDynamic->update(_instances.data(), _instances.size() * sizeof(InstanceData)); }

void ChessPiecesGLDraw::updateInstanceData() {
    _instances.clear();
    for (const auto &tile : _chessBoard.getBoard()) {
        if (!tile.hasPiece()) continue;

        glm::vec2 pos{tile.getX() * _pieceWidth, (boardHeight - 1 - tile.getY()) * _pieceHeight};
        float layer = _pieceTypeToLayer.at({tile.getPieceType(), tile.hasWhitePiece()});
        _instances.emplace_back(pos, layer);
    }
}
