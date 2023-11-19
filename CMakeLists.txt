cmake_minimum_required(VERSION 3.28)

project(Delusion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

FetchContent_Declare(
        Corrosion
        GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
        GIT_TAG v0.4.4
)
FetchContent_MakeAvailable(Corrosion)

add_subdirectory(thirdparty)

add_executable(Delusion src/main.cpp)
target_include_directories(Delusion PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Delusion PRIVATE glfw glfw3webgpu webgpu wgpu-native)
