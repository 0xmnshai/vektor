file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_rna INTERFACE)
target_sources(vektor_rna INTERFACE ${SRC_FILES})
target_include_directories(vektor_rna INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
