file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_windowmanager STATIC ${SRC_FILES})
target_include_directories(vektor_windowmanager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vektor_windowmanager PRIVATE fmt::fmt glfw glad)