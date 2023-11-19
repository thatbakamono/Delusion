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

add_subdirectory(Editor)
add_subdirectory(Engine)
add_subdirectory(thirdparty)
