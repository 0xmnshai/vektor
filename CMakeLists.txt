cmake_minimum_required(VERSION 4.2.3)

project(vektor VERSION 0.1.0 LANGUAGES CXX C)

# macOS specific configuration for Cocoa frameworks
if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Export compile commands for IDE/LSP (clangd) support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VEKTOR_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VEKTOR_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VEKTOR_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(VEKTOR_VERSION_CYCLE "alpha") # alpha, beta, rc, release

add_subdirectory(extern)
add_subdirectory(intern)

configure_file(
    "${PROJECT_SOURCE_DIR}/source/vektor_version.h.in"
    "${PROJECT_BINARY_DIR}/generated/vektor_version.h"
)

include_directories("${PROJECT_BINARY_DIR}/generated")

option(VEKTOR_BUILD_EDITOR "Build the Vektor editor" ON)
option(VEKTOR_BUILD_RUNTIME "Build the Vektor runtime" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source/runtime)

if(VEKTOR_BUILD_EDITOR)
    add_subdirectory(source/editor)
endif()