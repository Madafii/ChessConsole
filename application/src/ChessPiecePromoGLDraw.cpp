#include "ChessPiecePromoGLDraw.h"
#include "ChessTile.h"

namespace {
constexpr std::array promotionChoices{'Q', 'R', 'B', 'N'};
}

ChessPiecePromoGLDraw::ChessPiecePromoGLDraw()
    : _instances(nullptr, promotionChoices.size() * sizeof(ChessPieceData::InstanceData)) {
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(1);
    _renderData.VAO.AddBuffer(_instances, layout);
    _shader.Bind();
    _shader.SetUniform1i("u_Texture", 0);
    _shader.SetUniform1i("u_Promotion", 1);
    _shader.SetUniformMat4f("u_MVP", _renderData.mvp);
}

void ChessPiecePromoGLDraw::show(const ChessTile &destination, bool white) {
    using namespace ChessPieceData;
    _destination.emplace(destination.getX(), destination.getY());
    const auto [x, y] = *_destination;
    const int direction = y == 0 ? 1 : -1;
    std::array<InstanceData, promotionChoices.size()> instances;
    for (int i = 0; i < static_cast<int>(instances.size()); ++i) {
        const auto type = ChessPiece::getTypeFromShort(promotionChoices[i]);
        instances[i] = {{x * _tileWidth, (y + i * direction) * _tileHeight}, getLayer(type, white)};
    }
    _instances.update(instances.data(), sizeof(instances));
}

void ChessPiecePromoGLDraw::onRender() const {
    if (!_destination) return;

    _renderer.DrawDynamic(_renderData.VAO, _renderData.IBO, _shader, promotionChoices.size());
}

std::optional<char> ChessPiecePromoGLDraw::choiceAt(int x, int y) const {
    if (!_destination || x != _destination->first) return std::nullopt;

    const int rank = _destination->second;
    const int choice = rank == 0 ? y : rank - y;
    if (choice < 0 || choice >= static_cast<int>(promotionChoices.size())) return std::nullopt;
    return promotionChoices[choice];
}
