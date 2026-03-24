#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <sys/types.h>

class ChessGL {
  public:
    explicit ChessGL() = default;

    int start();

    static constexpr int windowHeight = 800;
    static constexpr int windowWidth = 800;

  private:
    static void error_callback(int error, const char *description);
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
};

