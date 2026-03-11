find_program(CARGO_EXECUTABLE cargo REQUIRED)

function(cxxbridge_generate_header)
    set(options)
    set(oneValueArgs SOURCE OUTPUT_DIR TARGET_DIR OUTPUT_VAR TARGET_NAME)
    set(multiValueArgs)
    cmake_parse_arguments(CXXBRIDGE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CXXBRIDGE_SOURCE)
        message(FATAL_ERROR "cxxbridge_generate_header: SOURCE must be specified")
    endif()
    if(NOT CXXBRIDGE_OUTPUT_DIR)
        message(FATAL_ERROR "cxxbridge_generate_header: OUTPUT_DIR must be specified")
    endif()
    if(NOT CXXBRIDGE_TARGET_DIR)
        message(FATAL_ERROR "cxxbridge_generate_header: TARGET_DIR must be specified")
    endif()

    get_filename_component(SRC_NAME "${CXXBRIDGE_SOURCE}" NAME)
    set(HEADER_OUT "${CXXBRIDGE_OUTPUT_DIR}/${SRC_NAME}.h")

    add_custom_command(
        OUTPUT "${HEADER_OUT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CXXBRIDGE_OUTPUT_DIR}"
        COMMAND ${CARGO_EXECUTABLE} install cxxbridge-cmd --root "${CXXBRIDGE_TARGET_DIR}" --quiet
        COMMAND "${CXXBRIDGE_TARGET_DIR}/bin/cxxbridge" "${CXXBRIDGE_SOURCE}" --header > "${HEADER_OUT}"
        DEPENDS "${CXXBRIDGE_SOURCE}"
        COMMENT "Generating C++ header for ${CXXBRIDGE_SOURCE}"
    )

    if(CXXBRIDGE_TARGET_NAME)
        add_custom_target(${CXXBRIDGE_TARGET_NAME} DEPENDS "${HEADER_OUT}")
    endif()

    if(CXXBRIDGE_OUTPUT_VAR)
        set(${CXXBRIDGE_OUTPUT_VAR} "${HEADER_OUT}" PARENT_SCOPE)
    endif()
endfunction()
