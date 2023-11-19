add_subdirectory(glfw)
add_subdirectory(glfw3webgpu)
add_subdirectory(webgpu-headers)

corrosion_import_crate(MANIFEST_PATH wgpu-native/Cargo.toml)

target_include_directories(wgpu-native INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/ffi")
target_include_directories(wgpu-native INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/ffi/webgpu-headers")

if(WIN32)
target_link_libraries(wgpu-native INTERFACE d3dcompiler.lib opengl32.lib)
endif()
