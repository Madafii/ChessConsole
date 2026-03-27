#include "ChessGL.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <memory>

// ChessGL::ChessGL() : _chessBoardDraw(), _chessPiecesDraw() {}
ChessGL::ChessGL() = default;

int ChessGL::start() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(windowHeight, windowWidth, "Chess Game", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, this);
    auto keyCallbackFunc = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        static_cast<ChessGL *>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
    };
    glfwSetKeyCallback(window, keyCallbackFunc);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "not ok!" << std::endl;
    }

    _chessBoardDraw = std::make_unique<ChessBoardGLDraw>();
    _chessPiecesDraw = std::make_unique<ChessPiecesGLDraw>(_chessInterface.getChessBoard());

    while (!glfwWindowShouldClose(window)) {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        _chessBoardDraw->OnRender();
        _chessPiecesDraw->OnRender();

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
    } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        _chessPiecesDraw->OnUpdate();
    }
}
