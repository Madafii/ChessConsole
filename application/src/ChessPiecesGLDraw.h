#pragma once

#include "ChessBoard.h"
#include "ChessInterface.h"
#include "ChessPiece.h"
#include "ChessPieceSelectedGLDraw.h"
#include "Renderer.h"
#include "TextureSet.h"
#include "VertexBufferDynamic.h"
#include <memory>
#include <vector>

class ChessPiecesGLDraw {
  public:
    explicit ChessPiecesGLDraw(ChessInterface &interface);

    void onRender();
    void onUpdate(double xPos, double yPos);
    void onClick(double xPos, double yPos);
    void onDrop(double xPos, double yPos);

  private:
    ChessInterface &_chessInterface;

    std::shared_ptr<SharedPieceRenderData> _renderData = std::make_shared<SharedPieceRenderData>();
    std::unique_ptr<Renderer> _renderer = std::make_unique<Renderer>();
    std::unique_ptr<TextureSet> _textureSet =
        std::make_unique<TextureSet>("application/res/textures/piecesSet.png", _pieceSetSize, _pieceSetRows);
    std::unique_ptr<Shader> _shader = std::make_unique<Shader>("application/res/shaders/Piece.shader");
    std::unique_ptr<VertexBufferDynamic> _vertexBufferDynamic;

    ChessPieceSelectedGLDraw _selected;
    PieceTiles _possibleMoves;

    std::vector<InstanceData> _instances;

    std::optional<std::pair<int, int>> screenToBoard(double xPos, double yPos);
    std::optional<InstanceData *> getInstance(double xPos, double yPos);

    bool inTileArea(glm::vec2 tilePos, double xPos, double yPos);

    void unselect();

    void eraseInstance(const InstanceData &data);
    void updateInstanceData();
    void updateInstanceBuffer();

    ChessTile *boardToTile(const std::pair<int, int> &pos) { return &_chessInterface.getChessBoard().getTileAt(pos); }

    static constexpr float getLayer(ChessPieceType type, bool white) {
        return static_cast<int>(type) + (white ? 6.0f : 0.0f);
    }
};
