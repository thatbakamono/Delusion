add_library(avutil INTERFACE)
target_include_directories(avutil INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(avutil INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/debug/avutil.lib")

        add_custom_target(
                avutil_copy
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/debug/avutil-58.dll" "${CMAKE_BINARY_DIR}/avutil-58.dll"
        )
    else ()
        target_link_libraries(avutil INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/release/avutil.lib")

        add_custom_target(
                avutil_copy
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/release/avutil-58.dll" "${CMAKE_BINARY_DIR}/avutil-58.dll"
        )
    endif ()

    add_dependencies(avutil avutil_copy)
else ()
    error("Unsupported platform")
endif ()
