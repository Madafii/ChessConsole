#pragma once

#include "ChessPieceData.h"
#include "Renderer.h"
#include "VertexBufferDynamic.h"
#include <optional>

class ChessPiecePromoGLDraw {
  public:
    ChessPiecePromoGLDraw();

    void show(const ChessTile &destination, bool white);
    void hide() { _destination.reset(); }
    void onRender() const;
    std::optional<char> choiceAt(int x, int y) const;

  private:
    ChessPieceData::SharedPieceRenderData _renderData;
    VertexBufferDynamic _instances;
    Renderer _renderer;
    Shader _shader{"application/res/shaders/Selected.shader"};
    std::optional<std::pair<int, int>> _destination;
};
