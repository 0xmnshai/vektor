file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_libmv INTERFACE)
target_sources(vektor_libmv INTERFACE ${SRC_FILES})
target_include_directories(vektor_libmv INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
