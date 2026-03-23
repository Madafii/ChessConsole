#pragma once

#include "Renderer.h"
#include <sys/types.h>

class Texture {
  private:
    uint m_RendererID;
    std::string m_FilePath;
    u_char *m_LocalBuffer;
    int m_Width, m_Height, m_BPP;

  public:
    Texture(const std::string &path);
    ~Texture();

    void Bind(uint slot = 0) const;
    void Unbind() const;

    inline int getWidth() const { return m_Width; }
    inline int getHeight() const { return m_Height; }
};
