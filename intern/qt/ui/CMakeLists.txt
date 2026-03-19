file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vektor_qt_ui STATIC ${SRC})

set_target_properties(vektor_qt_ui PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vektor_qt_ui PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog config)
# If common headers are elsewhere, add them here or via main target 