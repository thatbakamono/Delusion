add_library(avformat INTERFACE)
target_include_directories(avformat INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(avformat INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/debug/avformat.lib")

        add_custom_target(
            avformat_copy
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/debug/avformat-60.dll" "${CMAKE_BINARY_DIR}/avformat-60.dll"
        )
    else ()
        target_link_libraries(avformat INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/release/avformat.lib")

        add_custom_target(
            avformat_copy
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/release/avformat-60.dll" "${CMAKE_BINARY_DIR}/avformat-60.dll"
        )
    endif ()

    add_dependencies(avformat avformat_copy)
else ()
    error("Unsupported platform")
endif ()
