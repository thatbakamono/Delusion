add_executable(Editor src/main.cpp src/Editor.cpp)
target_include_directories(Editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Editor PRIVATE Engine imgui imgui-glfw imgui-wgpu nfd)

if (MSVC)
    target_compile_options(Editor PRIVATE /W4)
else ()
    target_compile_options(Editor PRIVATE -Wall -Wextra -Wpedantic)
endif ()
