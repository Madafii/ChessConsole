#pragma once

#include "ChessBoard.h"
#include "ChessInterface.h"
#include "ChessPiece.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "TextureSet.h"
#include "VertexArray.h"
#include "VertexBufferDynamic.h"
#include <memory>
#include <unordered_map>
#include <vector>

class ChessPiecesGLDraw {
  public:
    struct InstanceData {
        glm::vec2 position;
        float layer;
    };

    struct SelectedPiece {
        const ChessTile *tile;
        InstanceData *data;
    };

    explicit ChessPiecesGLDraw(ChessInterface &interface);

    void onRender();
    void onUpdate(double xPos, double yPos);
    void onClick(double xPos, double yPos);
    void onDrop(double xPos, double yPos);

    void updateInstanceData();

  private:
    ChessInterface &_chessInterface;
    std::unique_ptr<VertexArray> _vertexArray;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<VertexBufferDynamic> _vertexBufferDynamic;
    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::unique_ptr<Shader> _shader;
    std::unique_ptr<TextureSet> _textureSet;
    glm::vec3 _translation{0.0f, 0.0f, 0.0f};
    glm::mat4 _proj;
    glm::mat4 _view;
    std::vector<InstanceData> _instances;
    std::optional<SelectedPiece> _selected;
    PieceTiles _possibleMoves;

    const std::map<std::pair<ChessPieceType, bool>, float> _pieceTypeToLayer{
        {{ChessPieceType::PAWN, false}, 0.0f},  {{ChessPieceType::KNIGHT, false}, 1.0f},  {{ChessPieceType::BISHOP, false}, 2.0f},
        {{ChessPieceType::ROOK, false}, 3.0f},  {{ChessPieceType::KING, false}, 4.0f},    {{ChessPieceType::QUEEN, false}, 5.0f},
        {{ChessPieceType::PAWN, true}, 6.0f}, {{ChessPieceType::KNIGHT, true}, 7.0f}, {{ChessPieceType::BISHOP, true}, 8.0f},
        {{ChessPieceType::ROOK, true}, 9.0f}, {{ChessPieceType::KING, true}, 10.0f},  {{ChessPieceType::QUEEN, true}, 11.0f}};

    static constexpr float _tileHeight = 1.0f / boardHeight;
    static constexpr float _tileWidth = 1.0f / boardWidth;
    static constexpr float _pieceHeight = _tileHeight;
    static constexpr float _pieceWidth = _tileWidth;
    static constexpr size_t _pieceSetSize = 12;
    static constexpr size_t _pieceSetRows = 2;

    std::optional<std::pair<int, int>> screenToBoard(double xPos, double yPos);
    ChessTile *boardToTile(const std::pair<int, int> &pos) { return &_chessInterface.getChessBoard().getTileAt(pos); }
    void unselect();

    bool inTileArea(glm::vec2 tilePos, double xPos, double yPos);
    std::optional<InstanceData *> getInstance(double xPos, double yPos);
};
