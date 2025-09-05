
include(DrunkFly/Common)

if(DOS)
    set(STATIC_BUILD TRUE)
endif()

function(my_add_library target)

    string(TOUPPER "${target}" _target_upper)

    foreach(_file ${ARGN})
        get_filename_component(dir "${_file}" DIRECTORY)
        string(REPLACE "/" "\\" dir "Source Files/${dir}")
        source_group("${dir}" FILES "${_file}")
        list(APPEND src "${target}/${_file}")
    endforeach()

    if(STATIC_BUILD)
        add_library(${target} STATIC ${src})
        target_compile_definitions(${target} PUBLIC ${_target_upper}_STATIC)
    else()
        add_library(${target} SHARED ${src})
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
        target_compile_definitions(${target} PRIVATE ${_target_upper}_EXPORTS)
    endif()

    target_include_directories(${target} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

    if(MINGW)
        set_target_properties(${target} PROPERTIES
            PREFIX ""
            POSITION_INDEPENDENT_CODE OFF
            )
    endif()

    if(NOT "${target}" STREQUAL "runtime")
        target_link_libraries(${target} PUBLIC runtime)
    endif()

endfunction()
