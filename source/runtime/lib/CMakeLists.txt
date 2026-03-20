file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(lib STATIC ${SRC})

target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(lib PUBLIC ${CMAKE_BINARY_DIR}/generated)

target_link_libraries(lib PUBLIC glm)