#include "ChessGL.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"

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

    glDepthFunc(GL_LEQUAL);

    glfwSetWindowUserPointer(window, this);

    auto keyCallbackFunc = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        static_cast<ChessGL *>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
    };
    glfwSetKeyCallback(window, keyCallbackFunc);

    auto cursorCallbackFunc = [](GLFWwindow *window, double xpos, double ypos) {
        static_cast<ChessGL *>(glfwGetWindowUserPointer(window))->cursor_position_callback(window, xpos, ypos);
    };
    glfwSetCursorPosCallback(window, cursorCallbackFunc);

    auto mouseCallbackFunc = [](GLFWwindow *window, int button, int action, int mods) {
        static_cast<ChessGL *>(glfwGetWindowUserPointer(window))->mouse_button_callback(window, button, action, mods);
    };
    glfwSetMouseButtonCallback(window, mouseCallbackFunc);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) { glViewport(0, 0, width, height); });

    if (glewInit() != GLEW_OK) {
        std::cout << "not ok!" << std::endl;
    }

    _chessBoardDraw = std::make_unique<ChessBoardGLDraw>();
    _chessPiecesDraw = std::make_unique<ChessPiecesGLDraw>(_chessInterface);

    while (!glfwWindowShouldClose(window)) {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        _chessBoardDraw->OnRender();
        _chessPiecesDraw->onRender();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    _chessPiecesDraw.reset();
    _chessBoardDraw.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void ChessGL::error_callback([[maybe_unused]] int error, const char *description) { fprintf(stderr, "Error: %s\n", description); }

void ChessGL::key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void ChessGL::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (width > 0 && height > 0) _chessPiecesDraw->onUpdate(xpos / width, ypos / height);
}

void ChessGL::mouse_button_callback(GLFWwindow *window, int button, int action, [[maybe_unused]] int mods) {
    double xPos;
    double yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (width <= 0 || height <= 0) return;
    double normalizedXPos = xPos / width;
    double normalizedYPos = yPos / height;
    if (action == GLFW_PRESS) {
        _chessPiecesDraw->onClick(normalizedXPos, normalizedYPos);
    } else if (action == GLFW_RELEASE) {
        _chessPiecesDraw->onDrop(normalizedXPos, normalizedYPos);
    }
}
