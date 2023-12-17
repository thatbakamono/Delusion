set(BOX2D_BUILD_UNIT_TEST OFF)
set(BOX2D_BUILD_TESTBED OFF)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d
    GIT_TAG v2.4.1
)

FetchContent_MakeAvailable(box2d)

add_subdirectory(entt)
add_subdirectory(filewatch)
add_subdirectory(glfw)
add_subdirectory(glfw3webgpu)
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(mono)
add_subdirectory(nativefiledialog-extended)
add_subdirectory(webgpu-headers)

corrosion_import_crate(MANIFEST_PATH wgpu-native/Cargo.toml)

target_include_directories(wgpu-native INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/ffi")
target_include_directories(wgpu-native INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/ffi/webgpu-headers")

if(WIN32)
target_link_libraries(wgpu-native INTERFACE d3dcompiler.lib opengl32.lib)
endif()

add_subdirectory(yaml-cpp)
