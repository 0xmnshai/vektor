file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_atomic INTERFACE)
target_sources(vektor_atomic INTERFACE ${SRC_FILES})
target_include_directories(vektor_atomic INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
