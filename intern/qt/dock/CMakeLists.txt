file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vektor_qt_dock STATIC ${SRC})

set_target_properties(vektor_qt_dock PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_dock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# If common headers are elsewhere, add them here or via main target
add_subdirectory(common)
add_subdirectory(scene)
add_subdirectory(assets_browser)
add_subdirectory(console)
add_subdirectory(outliner)
add_subdirectory(properties)

target_link_libraries(vektor_qt_dock PUBLIC 
    vektor_qt_dock_common
    vektor_qt_dock_scene 
    vektor_qt_dock_assets_browser
    vektor_qt_dock_console
    vektor_qt_dock_outliner
    vektor_qt_dock_properties
    Qt6::Core 
    Qt6::Widgets 
    Qt6::OpenGLWidgets)