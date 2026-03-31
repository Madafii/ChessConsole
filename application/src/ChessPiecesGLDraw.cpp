#include "ChessPiecesGLDraw.h"
#include "ChessBoard.h"
#include "Renderer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>

ChessPiecesGLDraw::ChessPiecesGLDraw(ChessInterface &interface) : _chessInterface(interface) {
    float squareVertices[] = {
        0.0f,       0.0f,        0.0f, 0.0f, // 0
        _tileWidth, 0.0f,        1.0f, 0.0f, // 1
        _tileWidth, _tileHeight, 1.0f, 1.0f, // 2
        0.0f,       _tileHeight, 0.0f, 1.0f  // 3
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

void ChessPiecesGLDraw::onRender() {
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

void ChessPiecesGLDraw::onUpdate(double xPos, double yPos) {
    if (_selected) {
        _selected.value().data->position.x = xPos - _tileWidth / 2;
        _selected.value().data->position.y = 1.0 - yPos - _tileHeight / 2;
    }

    _vertexBufferDynamic->update(_instances.data(), _instances.size() * sizeof(InstanceData));
}

void ChessPiecesGLDraw::onClick(double xPos, double yPos) {
    std::cout << "clicked at: " << xPos << " : " << yPos << std::endl;
    unselect();

    // select instance of InstanceData
    auto selectedData = getInstance(xPos, yPos);
    if (!selectedData) return;

    // select board tile
    const auto fromTile = boardToTile(*screenToBoard(xPos, yPos));

    // check for possible moves
    auto possMoves = _chessInterface.getPossibleMovesFromTile(fromTile->getPos());
    if (!possMoves || possMoves.value().empty()) return;

    // select piece and cache possible moves
    _selected = SelectedPiece{fromTile, *selectedData};
    _possibleMoves = std::move(*possMoves);
}

void ChessPiecesGLDraw::onDrop(double xPos, double yPos) {
    std::cout << "dropped at: " << xPos << " : " << yPos << std::endl;
    // check if the screen pos in valid range
    auto boardPos = screenToBoard(xPos, yPos);
    if (!boardPos) return;

    // get tile to drop on
    auto toTile = boardToTile(*boardPos);

    // check if it is a valid tile to drop on
    for (const auto &tile : _possibleMoves) {
        if (toTile == tile) {
            const std::string move = std::format("{}:{}", _selected.value().tile->getPos(), toTile->getPos());
            _chessInterface.handleMoveInput(move);
        }
    }

    unselect();
    updateInstanceData();
    onUpdate(xPos, yPos);
}

void ChessPiecesGLDraw::updateInstanceData() {
    _instances.clear();
    for (const auto &tile : _chessInterface.getChessBoard().getBoard()) {
        if (!tile.hasPiece()) continue;

        glm::vec2 pos{tile.getX() * _tileWidth, tile.getY() * _tileHeight};
        float layer = _pieceTypeToLayer.at({tile.getPieceType(), tile.hasWhitePiece()});
        _instances.emplace_back(pos, layer);
    }
}

std::optional<std::pair<int, int>> ChessPiecesGLDraw::screenToBoard(double xPos, double yPos) {
    int x = static_cast<int>(xPos * boardWidth);
    int y = boardHeight - 1 - static_cast<int>(yPos * boardHeight);

    std::cout << "board pos: " << x << " : " << y << std::endl;

    if (!ChessBoard::validTilePos(x, y)) return std::nullopt;
    return {{x, y}};
}

void ChessPiecesGLDraw::unselect() {
    _selected = std::nullopt;
    _possibleMoves.clear();
}

bool ChessPiecesGLDraw::inTileArea(glm::vec2 tilePos, double xPos, double yPos) {
    return ((xPos <= tilePos.x + _tileWidth && xPos >= tilePos.x) && 1.0 - yPos <= tilePos.y + _tileHeight && 1.0 - yPos >= tilePos.y);
}

std::optional<ChessPiecesGLDraw::InstanceData *> ChessPiecesGLDraw::getInstance(double xPos, double yPos) {
    for (auto &instance : _instances) {
        if (inTileArea(instance.position, xPos, yPos)) {
            return &instance;
        }
    }
    return std::nullopt;
}
