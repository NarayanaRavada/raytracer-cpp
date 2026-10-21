cmake_minimum_required(VERSION 3.15)

project(ray_tracer)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

set(SOURCES
    src/main.cpp
    src/stb_image.cpp
)

add_executable(
    ray_tracer
    ${SOURCES}
)

# -------------------------
# SDL2
# -------------------------

find_package(SDL2 REQUIRED)

target_include_directories(ray_tracer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    ray_tracer
    PRIVATE SDL2::SDL2
)

# -------------------------
# Compiler options
# -------------------------

target_compile_definitions(ray_tracer PRIVATE
    ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

target_compile_options(
    ray_tracer
    PRIVATE
    -O3
    -march=native
    -ffast-math
)
