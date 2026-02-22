file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_translation INTERFACE)
target_sources(vektor_translation INTERFACE ${SRC_FILES})
target_include_directories(vektor_translation INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
