cmake_minimum_required(VERSION 3.16)

project(vektor VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(VEKTOR_BUILD_EDITOR "Build the Vektor editor" ON)
option(VEKTOR_BUILD_SANDBOX "Build the Vektor sandbox" ON)
option(VEKTOR_BUILD_TESTS "Build Tests" ON)

add_subdirectory(source/runtime)

if(VEKTOR_BUILD_EDITOR)

endif()

add_subdirectory(source/creator)


if(VEKTOR_BUILD_SANDBOX)

endif()