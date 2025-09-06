
include(DrunkFly/Common)

if(MSDOS OR EMSCRIPTEN OR OLD_BORLAND OR OLD_WATCOM)
    set(STATIC_BUILD TRUE)
endif()

gcc_visibility_hidden()

#
# Enforce C89 on some platforms, while excluding the problematic ones.
#
function(safe_force_c89)

    if(NOT CPU64 AND NOT EMSCRIPTEN AND NOT DJGPP)
        force_c89()
    endif()

endfunction()

#
# Produce application library
#
#   create_library(name [STATIC] sources)
#
function(create_library _target)

    set(_single)
    set(_multi)
    set(_options STATIC)
    cmake_parse_arguments(_opt "${_options}" "${_single}" "${_multi}" ${ARGN})

    if(STATIC_BUILD)
        set(_opt_STATIC TRUE)
    endif()

    string(TOUPPER "${_target}" _target_upper)
    string(REPLACE "-" "_" _target_upper "${_target_upper}")

    if(${_opt_STATIC})
        add_library(${_target} STATIC ${_opt_UNPARSED_ARGUMENTS})
        target_compile_definitions(${_target} PUBLIC ${_target_upper}_STATIC)
    else()
        add_library(${_target} SHARED ${_opt_UNPARSED_ARGUMENTS})
        set_target_properties(${_target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
        target_compile_definitions(${_target} PRIVATE ${_target_upper}_EXPORTS)
    endif()

    target_include_directories(${_target} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

    if(MINGW AND NOT ${_opt_STATIC})
        set_target_properties(${_target} PROPERTIES
            PREFIX ""
            POSITION_INDEPENDENT_CODE OFF
            )
    endif()

    if(NOT "${_target}" STREQUAL "runtime")
        target_link_libraries(${_target} PUBLIC runtime)
    endif()

endfunction()

#
# Produce console executable
#
#   create_console_executable(name sources)
#
function(create_console_executable _target)
    add_executable(${_target} ${ARGN})
    set_target_properties(${_target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_MAIN)
    target_link_libraries(${_target} PRIVATE runtime)
endfunction()

#
# Produce GUI executable
#
#   create_gui_executable(name sources)
#
function(create_gui_executable _target)
    add_executable(${_target} WIN32 MACOSX_BUNDLE ${ARGN})
    set_target_properties(${_target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_link_libraries(${_target} PRIVATE runtime)
    if(WIN32)
        target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_WINMAIN)
    else()
        target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_MAIN)
    endif()
endfunction()

######################################################################################################################

if(NOT "${PROJECT_NAME}" STREQUAL "Runtime")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../.." runtime)
endif()
