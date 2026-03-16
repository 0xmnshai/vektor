file(GLOB SRC "*.h" "*.cc" "intern/*.cc" "intern/*.hh")

add_library(gaurdalloc STATIC ${SRC})

target_include_directories(gaurdalloc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(gaurdalloc PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
