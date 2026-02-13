file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_clog STATIC ${SRC_FILES})
target_include_directories(vektor_clog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vektor_clog PUBLIC vektor_guardedalloc)