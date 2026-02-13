file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_cycles_util INTERFACE)
target_sources(vektor_cycles_util INTERFACE ${SRC_FILES})
target_include_directories(vektor_cycles_util INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
