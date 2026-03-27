#pragma once

#include "ChessBoard.h"
#include "ChessPiece.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "TextureSet.h"
#include "VertexArray.h"
#include "VertexBufferDynamic.h"
#include <memory>

class ChessPiecesGLDraw {
  public:
    struct InstanceData {
        glm::vec2 position;
        float layer;
    };

    explicit ChessPiecesGLDraw(ChessBoard &board);

    void OnRender();
    void OnUpdate();

    void updateInstanceData();

  private:
    ChessBoard &_chessBoard;
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
    std::map<std::pair<ChessPieceType, bool>, float> _pieceTypeToLayer{
        {{ChessPieceType::PAWN, true}, 0.0f},  {{ChessPieceType::KNIGHT, true}, 1.0f},  {{ChessPieceType::BISHOP, true}, 2.0f},
        {{ChessPieceType::ROOK, true}, 3.0f},  {{ChessPieceType::KING, true}, 4.0f},    {{ChessPieceType::QUEEN, true}, 5.0f},
        {{ChessPieceType::PAWN, false}, 6.0f}, {{ChessPieceType::KNIGHT, false}, 7.0f}, {{ChessPieceType::BISHOP, false}, 8.0f},
        {{ChessPieceType::ROOK, false}, 9.0f}, {{ChessPieceType::KING, false}, 10.0f},  {{ChessPieceType::QUEEN, false}, 11.0f}};

    static constexpr float _pieceHeight = 1.0f / boardHeight;
    static constexpr float _pieceWidth = 1.0f / boardWidth;
    static constexpr size_t _pieceSetSize = 12;
    static constexpr size_t _pieceSetRows = 2;
};
