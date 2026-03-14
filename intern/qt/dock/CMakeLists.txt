file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vektor_qt_dock STATIC ${SRC})

set_target_properties(vektor_qt_dock PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_dock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# If common headers are elsewhere, add them here or via main target
add_subdirectory(scene)

target_link_libraries(vektor_qt_dock PUBLIC vektor_qt_dock_scene Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets)