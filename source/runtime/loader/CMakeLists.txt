file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_loader INTERFACE)
target_sources(vektor_loader INTERFACE ${SRC_FILES})
target_include_directories(vektor_loader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
