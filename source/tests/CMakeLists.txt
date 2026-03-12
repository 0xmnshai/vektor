
add_executable(vpi_event_test vpi_event_test.cc)

target_include_directories(vpi_event_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/intern/vpi
    ${CMAKE_SOURCE_DIR}/intern/vpi/intern
    ${CMAKE_BINARY_DIR}/generated
)

target_link_libraries(vpi_event_test PRIVATE
    vpi
    GTest::gtest_main
)

if(APPLE)
    target_link_libraries(vpi_event_test PUBLIC "-framework Cocoa")
endif()

include(GoogleTest)
gtest_discover_tests(vpi_event_test)
