#include "TextureSet.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "stb_image.h"

TextureSet::TextureSet(const std::string &path, uint ssize, uint srows)
    : _rendererID(0), _setSize(ssize), _setRows(srows), _filePath(path), _localBuffer(nullptr), _width(0), _height(0), _bpp(0) {
    stbi_set_flip_vertically_on_load(1);
    _localBuffer = stbi_load(path.c_str(), &_width, &_height, &_bpp, 4);

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    int cols = _setSize / _setRows;
    int pieceWidth = _width / cols;
    int pieceHeight = _height / _setRows;

    GLCall(glGenTextures(1, &_rendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, _rendererID));

    GLCall(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, pieceWidth, pieceHeight, _setSize));
    GLCall(glPixelStorei(GL_UNPACK_ROW_LENGTH, _width));

    for (uint layer = 0; layer < _setSize; layer++) {
        int row = layer / cols;
        int col = layer % cols;

        int xOffset = col * pieceWidth;
        int yOffset = row * pieceHeight;

        unsigned char *subData = _localBuffer + (yOffset * _width + xOffset) * 4;

        GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, pieceWidth, pieceHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, subData));
    }

    GLCall(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));

    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    Unbind();

    if (_localBuffer) {
        stbi_image_free(_localBuffer);
    }
}

TextureSet::~TextureSet() { GLCall(glDeleteTextures(1, &_rendererID)); }

void TextureSet::Bind(uint slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, _rendererID));
}

void TextureSet::Unbind() const { GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); }
