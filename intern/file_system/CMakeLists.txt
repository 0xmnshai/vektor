file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_filesystem STATIC ${SRC_FILES})
target_include_directories(vektor_filesystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

