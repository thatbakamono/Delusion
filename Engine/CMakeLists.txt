add_library(
        Engine
        src/MetadataSerde.cpp src/Scene.cpp src/SceneSerde.cpp src/formats/ImageDecoder.cpp
        src/graphics/GraphicsBackend.cpp src/graphics/Renderer.cpp src/graphics/Shader.cpp src/graphics/Texture2D.cpp
)
target_include_directories(Engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Engine PUBLIC cimage EnTT filewatch glfw glfw3webgpu glm webgpu wgpu-native yaml-cpp)
