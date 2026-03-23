add_executable(chessGL main.cpp)

add_subdirectory(src)

add_custom_target(copy_res ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/application/res
            $<TARGET_FILE_DIR:chessGL>/res
        )

target_compile_options(chessGL PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(chessGL PUBLIC chessGLBase)
