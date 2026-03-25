#pragma once

#include <string>

class TextureSet {
  public:
    explicit TextureSet(const std::string &path, uint ssize, uint srows);
    ~TextureSet();

    void Bind(uint slot = 0) const;
    void Unbind() const;

    inline int getWidth() const { return _width; }
    inline int getHeight() const { return _height; }

  private:
    uint _rendererID;
    uint _setSize, _setRows;
    std::string _filePath;
    u_char *_localBuffer;
    int _width, _height, _bpp;
};
