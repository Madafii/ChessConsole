#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
  private:
    std::string m_FilePath;
    uint m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocation;

  public:
    Shader(const std::string &filename);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform1i(const std::string &name, int value);
    void SetUniform1f(const std::string &name, float value);
    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string &name, const glm::mat4 &matrix);

  private:
    ShaderProgramSource ParseShader(const std::string &file);
    uint CompileShader(uint type, const std::string &source);
    uint CreateShader(const std::string &vertexShader, const std::string &fragmentShader);

    int GetUniformLocation(const std::string &name);
};
