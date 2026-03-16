add_library(vektor_qt INTERFACE)

# Add subdirectories
add_subdirectory(ui)
add_subdirectory(dock)
add_subdirectory(menu)

target_link_libraries(vektor_qt 
  INTERFACE 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::OpenGLWidgets
    vektor_qt_ui
    vektor_qt_dock
    vektor_qt_menu
)
