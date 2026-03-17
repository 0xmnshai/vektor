file(GLOB SRC "*.h" "*.cc" "intern/*.cc" "intern/*.hh")

add_library(vektor_qt_dock_common STATIC ${SRC})

set_target_properties(vektor_qt_dock_common PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_dock_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vektor_qt_dock_common PUBLIC 
    Qt6::Core 
    Qt6::Widgets
    Qt6::OpenGLWidgets
    vektor_qt_dock_scene
    vektor_qt_dock_outliner
    vektor_qt_dock_properties
    vektor_qt_dock_assets_browser
    vektor_qt_dock_console
)
