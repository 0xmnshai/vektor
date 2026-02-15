# Trigger re-scan
file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_executable(vektor ${SRC_FILES})
target_link_libraries(vektor PRIVATE vektor_kernel vektor_vklib vektor_dna vektor_clog vektor_windowmanager vektor_editor)
target_include_directories(vektor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(vektor PRIVATE
    VEKTOR_VERSION=100
    VEKTOR_VERSION_PATCH=0
)

if(APPLE)
    target_link_libraries(vektor PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
endif()
