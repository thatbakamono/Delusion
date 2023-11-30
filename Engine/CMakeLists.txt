add_library(Engine)
target_sources(
        Engine
        PUBLIC
        FILE_SET CXX_MODULES FILES
        src/Components.ixx src/Engine.ixx src/Image.ixx src/formats/ImageDecoder.ixx src/graphics/Renderer.ixx
        src/graphics/Shader.ixx src/graphics/Texture2D.ixx
)
target_include_directories(Engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Engine PUBLIC cimage EnTT glfw glfw3webgpu glm webgpu wgpu-native)
