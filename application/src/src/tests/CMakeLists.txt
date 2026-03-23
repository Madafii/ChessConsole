add_library(tests
    Test.h Test.cpp
    TestClearColor.h TestClearColor.cpp
    TestTexture2D.h TestTexture2D.cpp
)

target_include_directories(tests PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(tests PRIVATE imgui src)
