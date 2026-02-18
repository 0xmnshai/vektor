file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_glfw STATIC ${SRC_FILES})
target_include_directories(vektor_glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vektor_glfw PUBLIC glad glfw vektor_clog)
