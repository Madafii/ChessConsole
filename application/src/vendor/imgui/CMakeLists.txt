add_library(imgui
    imconfig.h
    imgui.h imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_impl_glfw.h imgui_impl_glfw.cpp
    imgui_impl_opengl3.h imgui_impl_opengl3.cpp
    imgui_impl_opengl3_loader.h
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR})
