add_library(mono INTERFACE)
target_include_directories(mono INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(mono INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/debug/libmono-static-sgen.lib")
else ()
    target_link_libraries(mono INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/release/libmono-static-sgen.lib")
endif ()

if (WIN32)
    target_link_libraries(mono INTERFACE Bcrypt.lib Version.lib Ws2_32.lib Winmm.lib)
else ()
    error("Unsupported platform")
endif ()
