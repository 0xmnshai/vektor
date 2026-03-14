file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(windowmanager STATIC ${SRC})

target_include_directories(windowmanager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(windowmanager PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(windowmanager PUBLIC ${CMAKE_BINARY_DIR}/generated)

target_link_libraries(windowmanager PUBLIC fmt::fmt Qt6::Core Qt6::Widgets Qt6::Gui)