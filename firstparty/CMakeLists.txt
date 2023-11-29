corrosion_import_crate(MANIFEST_PATH cimage/Cargo.toml)

target_include_directories(cimage INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/cimage")
