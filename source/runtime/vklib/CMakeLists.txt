file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_vklib STATIC ${SRC_FILES})
target_include_directories(vektor_vklib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
