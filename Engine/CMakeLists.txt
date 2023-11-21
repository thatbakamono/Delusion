add_library(Engine)
target_sources(
        Engine
        PUBLIC
        FILE_SET CXX_MODULES FILES
        src/Engine.ixx
)
target_link_libraries(Engine PUBLIC glfw glfw3webgpu webgpu wgpu-native)
