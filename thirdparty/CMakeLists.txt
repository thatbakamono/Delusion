option(BOX2D_BUILD_UNIT_TESTS "Build the Box2D unit tests" OFF)
option(BOX2D_BUILD_TESTBED "Build the Box2D testbed" OFF)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d
    GIT_TAG v2.4.1
)

FetchContent_MakeAvailable(box2d)

add_subdirectory(entt)
add_subdirectory(ffmpeg)
add_subdirectory(filewatch)
add_subdirectory(glfw)
add_subdirectory(glfw3webgpu)
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(mono)
add_subdirectory(nativefiledialog-extended)

FetchContent_Declare(
    portaudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio
    GIT_TAG v19.7.0
)

FetchContent_MakeAvailable(portaudio)

add_subdirectory(webgpu-headers)

corrosion_import_crate(MANIFEST_PATH wgpu-native/Cargo.toml)

target_include_directories(wgpu-native INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/ffi")
target_include_directories(wgpu-native INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/ffi/webgpu-headers")

if(WIN32)
target_link_libraries(wgpu-native INTERFACE d3dcompiler.lib opengl32.lib)
endif()

add_subdirectory(yaml-cpp)
