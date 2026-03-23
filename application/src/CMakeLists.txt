find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(chessGLBase
    ChessGL.h ChessGL.cpp
    Renderer.h Renderer.cpp
    VertexBuffer.h VertexBuffer.cpp
    IndexBuffer.h IndexBuffer.cpp
    VertexArray.h VertexArray.cpp
    VertexBufferLayout.h VertexBufferLayout.cpp
    Shader.h Shader.cpp
    Texture.h Texture.cpp
)

add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/imgui)

target_include_directories(chessGLBase PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chessGLBase glfw GLEW::GLEW OpenGL::GL glm::glm stb_image chessBase)
