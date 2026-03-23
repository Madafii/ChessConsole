#include "TestTexture2D.h"
#include "Renderer.h"
#include "imgui.h"
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>

namespace test {

TestTexture2D::TestTexture2D() : m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0) {
    float positions[] = {
        -50.0f, -50.0f, 0.0f, 0.0f, // 1
        50.0f,  -50.0f, 1.0f, 0.0f, // 2
        50.0f,  50.0f,  1.0f, 1.0f, // 3
        -50.0f, 50.0f,  0.0f, 1.0f  // 4
    };

    uint indices[] = {
        0, 1, 2, // 1
        2, 3, 0  // 2
    };

    m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    m_VAO->AddBuffer(*m_VertexBuffer, layout);
    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

    m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
    m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
    m_Texture = std::make_unique<Texture>("res/textures/monkey.png");
    m_Shader->SetUniform1i("u_Texture", 0);
}

TestTexture2D::~TestTexture2D() {}

void TestTexture2D::OnUpdate(float deltaTime) {}

void TestTexture2D::OnRender() {
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;

    m_Texture->Bind();

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }
}

void TestTexture2D::OnImGuiRender() {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SliderFloat3("TranslationA", &m_TranslationA.x, 0.0f, 960.0f);
    ImGui::SliderFloat3("TranslationB", &m_TranslationB.x, 0.0f, 960.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f, io.Framerate, io.Framerate);
}

} // namespace test
