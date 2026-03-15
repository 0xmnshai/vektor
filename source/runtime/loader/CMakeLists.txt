file(GLOB SRC "*.h" "*.cc" "*.mm" "intern/*.hh" "intern/*.cc")

add_library(loader STATIC ${SRC})
set_target_properties(loader PROPERTIES AUTOMOC ON)

target_include_directories(loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(loader PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(loader PUBLIC ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(loader PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog)