add_library(Engine)
target_sources(
        Engine
        PUBLIC
        FILE_SET CXX_MODULES FILES
        src/Engine.ixx src/Entity.ixx src/Image.ixx src/formats/ImageDecoder.ixx src/Scene.ixx src/graphics/Texture2D.ixx
)
target_link_libraries(Engine PUBLIC cimage EnTT glfw glfw3webgpu webgpu wgpu-native)
