cmake_minimum_required(VERSION 3.26)

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

find_program(
        DOTNET_EXECUTABLE
        NAMES dotnet
        HINTS "C:/Program Files/dotnet"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DOTNET_BUILD_TYPE "Debug")
else ()
    set(DOTNET_BUILD_TYPE "Release")
endif ()

add_custom_target(
        DelusionSharp
        COMMAND ${DOTNET_EXECUTABLE} build /p:Configuration=${DOTNET_BUILD_TYPE} /p:Platform="Any CPU"
                && ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DelusionSharp/bin/${DOTNET_BUILD_TYPE}/DelusionSharp.dll ${CMAKE_CURRENT_BINARY_DIR}/DelusionSharp.dll
                && ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DelusionSharp/bin/${DOTNET_BUILD_TYPE}/DelusionSharp.pdb ${CMAKE_CURRENT_BINARY_DIR}/DelusionSharp.pdb
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/DelusionSharp
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        add_compile_options(/fsanitize=address)
        add_link_options(/fsanitize=address)
    else ()
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
endif ()

add_subdirectory(Editor)
add_subdirectory(Engine)
add_subdirectory(firstparty)
add_subdirectory(thirdparty)
