file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_guardedalloc STATIC ${SRC_FILES})
target_include_directories(vektor_guardedalloc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vektor_guardedalloc PUBLIC vektor_atomic)
