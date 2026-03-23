#include "ChessGL.h"
#include "ChessBoard.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <cstdlib>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <memory>

int ChessGL::start() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int windowHeight = 800;
    int windowWidth = 800;
    GLFWwindow *window = glfwCreateWindow(windowHeight, windowWidth, "Chess Game", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "not ok!" << std::endl;
    }

    // CREATE CHESS BOARD
    const uint tileHeight = windowHeight / boardHeight;
    const uint tileWidth = windowWidth / boardWidth;
    const size_t boardPositionsSize = (boardWidth + 1) * (boardHeight + 1) * 2;
    std::array<float, boardPositionsSize> boardPositions;
    uint positionIndex = 0;
    for (int h = 0; h < boardHeight + 1; ++h) {
        for (int w = 0; w < boardWidth + 1; ++w) {
            boardPositions[positionIndex] = tileHeight * h;
            positionIndex++;
            boardPositions[positionIndex] = tileWidth * w;
            positionIndex++;
        }
    }

    const size_t indiceSize = boardSize * 2 * 3;
    std::array<uint, indiceSize> boardIndicies;
    uint rightFirstIndice = boardWidth + 1;
    uint rightSecondIndice = boardWidth + 2;
    uint rightThirdIndice = 1;
    uint leftFirstIndice = 1;
    uint leftSecondIndice = 0;
    uint leftThirdIndice = boardWidth + 1;

    for (size_t i = 0; i < indiceSize; i += 6) {
        boardIndicies[i] = rightFirstIndice;
        boardIndicies[i + 1] = rightSecondIndice;
        boardIndicies[i + 2] = rightThirdIndice;
        boardIndicies[i + 3] = leftFirstIndice;
        boardIndicies[i + 4] = leftSecondIndice;
        boardIndicies[i + 5] = leftThirdIndice;
        if (rightSecondIndice % (boardWidth + 1) != boardWidth) {
            rightFirstIndice++;
            rightSecondIndice++;
            rightThirdIndice++;
            leftFirstIndice++;
            leftSecondIndice++;
            leftThirdIndice++;
        } else {
            rightFirstIndice += 2;
            rightSecondIndice += 2;
            rightThirdIndice += 2;
            leftFirstIndice += 2;
            leftSecondIndice += 2;
            leftThirdIndice += 2;
        }
    }

    // ------------------------------------ //
    auto va = std::make_unique<VertexArray>();
    auto vb = std::make_unique<VertexBuffer>(&boardPositions, boardPositionsSize * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);

    va->AddBuffer(*vb, layout);
    IndexBuffer ib(&boardIndicies, indiceSize);

    Shader shader("application/res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    auto proj = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    glm::vec3 translationA(0, 0, 0);

    while (!glfwWindowShouldClose(window)) {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
            glm::mat4 mvp = proj * view * model;
            shader.Bind();
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*va, ib, shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void ChessGL::error_callback([[maybe_unused]] int error, const char *description) { fprintf(stderr, "Error: %s\n", description); }

void ChessGL::key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
