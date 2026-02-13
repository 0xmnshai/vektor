file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_kernel STATIC ${SRC_FILES})
target_link_libraries(vektor_kernel PUBLIC vektor_dna)
target_include_directories(vektor_kernel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
