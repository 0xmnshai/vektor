file(GLOB SRC "*.h" "*.cc")

add_library(clog STATIC ${SRC})

target_include_directories(clog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(clog PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(clog PUBLIC gaurdalloc)
