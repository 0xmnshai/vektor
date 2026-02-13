file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_dna INTERFACE)
target_sources(vektor_dna INTERFACE ${SRC_FILES})
target_include_directories(vektor_dna INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
