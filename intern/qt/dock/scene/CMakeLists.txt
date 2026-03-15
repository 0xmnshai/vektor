file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vektor_qt_dock_scene STATIC ${SRC})

set_target_properties(vektor_qt_dock_scene PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_dock_scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vektor_qt_dock_scene PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets gpu rna ecs lib)

