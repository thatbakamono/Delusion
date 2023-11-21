add_library(Engine)
target_sources(
        Engine
        PUBLIC
        FILE_SET CXX_MODULES FILES
        src/Engine.ixx src/Entity.ixx src/Scene.ixx
)
target_link_libraries(Engine PUBLIC EnTT glfw glfw3webgpu webgpu wgpu-native)
