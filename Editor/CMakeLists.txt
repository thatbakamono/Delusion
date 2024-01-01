add_executable(
        Editor
        src/main.cpp src/editor/Editor.cpp src/editor/ui/AssetBrowserPanel.cpp src/editor/ui/HierarchyPanel.cpp
        src/editor/ui/PropertiesPanel.cpp src/editor/ui/ViewportPanel.cpp
)
target_include_directories(Editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Editor PRIVATE Engine imgui imgui-glfw imgui-wgpu nfd)

if (MSVC)
    target_compile_options(Editor PRIVATE /W4)
else ()
    target_compile_options(Editor PRIVATE -Wall -Wextra -Wpedantic)
endif ()
