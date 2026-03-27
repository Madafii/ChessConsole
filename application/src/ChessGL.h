#pragma once

#include "ChessBoardGLDraw.h"
#include "ChessInterface.h"
#include "ChessPiecesGLDraw.h"
#include <GL/glew.h>
#include <memory>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <sys/types.h>

class ChessGL {
  public:
    explicit ChessGL();

    int start();

    static constexpr int windowHeight = 800;
    static constexpr int windowWidth = 800;

  private:
    ChessInterface _chessInterface;
    std::unique_ptr<ChessBoardGLDraw> _chessBoardDraw;
    std::unique_ptr<ChessPiecesGLDraw> _chessPiecesDraw;

    static void error_callback(int error, const char *description);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
};

