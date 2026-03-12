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
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

# Export compile commands for IDE/LSP (clangd) support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(UseCxxBridge)

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

if(VEKTOR_BUILD_RUNTIME)
    add_subdirectory(source/runtime)
endif()

if(VEKTOR_BUILD_EDITOR)
    add_subdirectory(source/editor)
endif()

add_subdirectory(source/tests)

set(COMPUTE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/compute")
set(COMPUTE_TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/compute_target")
set(COMPUTE_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/compute_generated")

file(MAKE_DIRECTORY ${COMPUTE_GEN_DIR})

find_program(CARGO_EXECUTABLE cargo REQUIRED)

set(INTERN_LIB_NAME "intern")
set(INTERN_LIB_PATH "${COMPUTE_TARGET_DIR}/debug/lib${INTERN_LIB_NAME}.a")

add_custom_command(
    OUTPUT ${INTERN_LIB_PATH}
    COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${COMPUTE_TARGET_DIR} 
            ${CARGO_EXECUTABLE} build -p ${INTERN_LIB_NAME}
    WORKING_DIRECTORY ${COMPUTE_SOURCE_DIR}
    COMMENT "Building Vektor Compute Intern COMPUTE Library"
)

add_custom_target(compute_intern_build ALL DEPENDS ${INTERN_LIB_PATH})

# an interface library for C++ to link against
add_library(compute_intern STATIC IMPORTED GLOBAL)
set_target_properties(compute_intern PROPERTIES
    IMPORTED_LOCATION ${INTERN_LIB_PATH}
    INTERFACE_INCLUDE_DIRECTORIES ${COMPUTE_GEN_DIR}
)
add_dependencies(compute_intern compute_intern_build)

# Generate C++ headers using cxx-bridge via the reusable module
cxxbridge_generate_header(
    SOURCE "${COMPUTE_SOURCE_DIR}/intern/src/lib.rs"
    OUTPUT_DIR "${COMPUTE_GEN_DIR}/rust/intern/src"
    TARGET_DIR "${COMPUTE_TARGET_DIR}"
    TARGET_NAME rust_bridge_headers
)
add_dependencies(compute_intern compute_intern_build)