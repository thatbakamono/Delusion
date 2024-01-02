add_library(avcodec INTERFACE)
target_include_directories(avcodec INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(avcodec INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/debug/avcodec.lib")

        add_custom_target(
            avcodec_copy
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/debug/avcodec-60.dll" "${CMAKE_BINARY_DIR}/avcodec-60.dll"
        )
    else ()
        target_link_libraries(avcodec INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/release/avcodec.lib")

        add_custom_target(
            avcodec_copy
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/release/avcodec-60.dll" "${CMAKE_BINARY_DIR}/avcodec-60.dll"
        )
    endif ()

    add_dependencies(avcodec avcodec_copy)
else ()
    error("Unsupported platform")
endif ()
