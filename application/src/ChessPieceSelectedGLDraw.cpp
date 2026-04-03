#include "ChessPieceSelectedGLDraw.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <utility>

ChessPieceSelectedGLDraw::ChessPieceSelectedGLDraw(std::shared_ptr<SharedPieceRenderData> renderData) : _renderData(std::move(renderData)) {
    _vertexBufferDynamic = std::make_unique<VertexBufferDynamic>(nullptr, sizeof(InstanceData));

    VertexBufferLayout layoutDynamic;
    layoutDynamic.Push<float>(2); // positon board
    layoutDynamic.Push<float>(1); // layer

    _renderData->VAO.AddBuffer(*_vertexBufferDynamic, layoutDynamic);
}

void ChessPieceSelectedGLDraw::onRender() const {
    if (!_instance) return;

    _shader->Bind();
    _shader->SetUniform1i("u_Texture", 0);
    _shader->SetUniformMat4f("u_MVP", _renderData->mvp);
    _renderData->VAO.Bind();
    _renderData->IBO.Bind();
    _renderer->DrawDynamic(_renderData->VAO, _renderData->IBO, *_shader, 1);
}

void ChessPieceSelectedGLDraw::onUpdate() {
    if (!_instance) return;
    _vertexBufferDynamic->update(&_instance.value(), sizeof(InstanceData));
}
