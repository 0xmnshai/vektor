file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vpi STATIC ${SRC})

target_include_directories(vpi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(vpi PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(vpi PUBLIC ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(vpi PUBLIC Qt6::Core Qt6::Widgets)
