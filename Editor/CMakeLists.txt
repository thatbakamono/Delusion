add_executable(Editor src/main.cpp)
target_sources(
        Editor
        PUBLIC
        FILE_SET CXX_MODULES FILES
        src/Editor.cpp
)
target_include_directories(Editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Editor PRIVATE Engine imgui imgui-glfw imgui-wgpu)
