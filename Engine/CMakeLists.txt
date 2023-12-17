add_library(
        Engine
        src/MetadataSerde.cpp src/Scene.cpp src/SceneSerde.cpp src/formats/ImageDecoder.cpp
        src/graphics/GraphicsBackend.cpp src/graphics/Renderer.cpp src/graphics/Shader.cpp src/graphics/Texture2D.cpp
)
target_include_directories(Engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Engine PUBLIC box2d cimage EnTT filewatch glfw glfw3webgpu glm mono webgpu wgpu-native yaml-cpp)
target_compile_definitions(Engine PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_LEFT_HANDED)

if (MSVC)
    target_compile_options(Engine PRIVATE /W4)
else ()
    target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
endif ()

add_dependencies(Engine DelusionSharp)
