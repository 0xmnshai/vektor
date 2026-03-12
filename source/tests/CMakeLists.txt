
add_executable(tests_main tests_main.cc vpi_event_test.cc)

target_include_directories(tests_main PRIVATE 
    ${CMAKE_SOURCE_DIR}/intern/vpi
    ${CMAKE_SOURCE_DIR}/intern/vpi/intern
    ${CMAKE_BINARY_DIR}/generated
)

target_link_libraries(tests_main PRIVATE
    vpi
)

if(APPLE)
    target_link_libraries(tests_main PUBLIC "-framework Cocoa")
endif()
