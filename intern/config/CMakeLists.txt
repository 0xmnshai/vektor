file(GLOB SRC "*.h" "*.cc" "intern/*.h" "intern/*.cc")

add_library(config STATIC ${SRC})

target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
