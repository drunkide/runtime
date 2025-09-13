
include(DrunkFly/Common)

gcc_visibility_hidden()

if(MSDOS OR EMSCRIPTEN OR OLD_BORLAND OR OLD_WATCOM)
    set(STATIC_BUILD TRUE)
endif()

define_property(TARGET PROPERTY _TESTS
    BRIEF_DOCS "List of test sources attached to the target"
    FULL_DOCS "Custom property to keep track of test sources per target"
    )

set(OUTDIR "${CMAKE_BINARY_DIR}/../../build-artifacts")
# Platform ###########################################################################################################
if("${POUR_PLATFORM}" STREQUAL "html5")
    set(OUTDIR "${OUTDIR}/${POUR_PLATFORM}")
elseif("${POUR_PLATFORM}" STREQUAL "linux_x64")
    set(OUTDIR "${OUTDIR}/linux64")
elseif("${POUR_PLATFORM}" STREQUAL "linux_x86")
    set(OUTDIR "${OUTDIR}/linux32")
elseif("${POUR_PLATFORM}" STREQUAL "msdos")
    set(OUTDIR "${OUTDIR}/${POUR_PLATFORM}")
elseif("${POUR_PLATFORM}" STREQUAL "win32")
    set(OUTDIR "${OUTDIR}/${POUR_PLATFORM}")
elseif("${POUR_PLATFORM}" STREQUAL "win64")
    set(OUTDIR "${OUTDIR}/${POUR_PLATFORM}")
else()
    message(FATAL_ERROR "Unknown platform \"${POUR_PLATFORM}\". Please update Runtime.cmake.")
endif()
# Compiler ###########################################################################################################
if("${POUR_COMPILER}" MATCHES "^emsdk_")
    # don't add compiler name for HTML5
elseif("${POUR_COMPILER}" STREQUAL "clang_3.5.0")
    set(OUTDIR "${OUTDIR}/clang350")
elseif("${POUR_COMPILER}" STREQUAL "egcs_1.1.2")
    set(OUTDIR "${OUTDIR}/egcs112")
elseif("${POUR_COMPILER}" STREQUAL "djgpp_12.2.0")
    set(OUTDIR "${OUTDIR}/djgpp122")
elseif("${POUR_COMPILER}" STREQUAL "borland_4.5.2")
    set(OUTDIR "${OUTDIR}/borland4")
elseif("${POUR_COMPILER}" STREQUAL "clang_4.0.0")
    set(OUTDIR "${OUTDIR}/clang400")
elseif("${POUR_COMPILER}" STREQUAL "mingw_4.4.0")
    set(OUTDIR "${OUTDIR}/mingw440")
elseif("${POUR_COMPILER}" STREQUAL "mingw_8.1.0")
    set(OUTDIR "${OUTDIR}/mingw810")
elseif("${POUR_COMPILER}" STREQUAL "msvc_2.0")
    set(OUTDIR "${OUTDIR}/msvc20")
elseif("${POUR_COMPILER}" STREQUAL "msvc_4.1")
    set(OUTDIR "${OUTDIR}/msvc41")
elseif("${POUR_COMPILER}" STREQUAL "msvc_2022")
    set(OUTDIR "${OUTDIR}/msvs2022")
elseif("${POUR_COMPILER}" STREQUAL "watcom_10.0a")
    set(OUTDIR "${OUTDIR}/watcom10")
else()
    message(FATAL_ERROR "Unknown compiler \"${POUR_COMPILER}\". Please update Runtime.cmake.")
endif()
######################################################################################################################

set(CRTDLL_DLL "${CMAKE_CURRENT_LIST_DIR}/../../lib/crtdll/crtdll.dll")
set(CRTDLL_LIB "${CMAKE_CURRENT_LIST_DIR}/../../lib/crtdll/crtdll.lib")
set(MSVCRT32 "${CMAKE_CURRENT_LIST_DIR}/../../lib/crtdll/msvcrt32.lib")
set(MSVCRT64 "${CMAKE_CURRENT_LIST_DIR}/../../lib/crtdll/msvcrt64.lib")

#
# Enforce C89 on some platforms, while excluding the problematic ones.
#
function(safe_force_c89)

    if(NOT CPU64 AND NOT EMSCRIPTEN AND NOT DJGPP)
        force_c89()
    endif()

endfunction()

#
# Set target flags for compiler
#
macro(_apply_compiler_flags _target)

    if(MSVC AND NOT OLD_MSVC)
        target_compile_options(${_target} PRIVATE
            /GS-
            )
        if(CPU32)
            target_compile_options(${_target} PRIVATE
                /d2noftol3
                )
        endif()
    endif()

    if(MINGW AND CPU32)
        target_compile_options(${_target} PRIVATE
            -march=i586
            )
        target_compile_definitions(${_target} PRIVATE
            -DUSE_NO_MINGW_SETJMP_TWO_ARGS
            -D__NO_ISOCEXT
            )
    endif()

endmacro()

#
# Set target flags for linker
#
macro(_apply_linker_flags _target _type)

    if(NOT WIN32)
        if("${_type}" STREQUAL "CONSOLE_EXECUTABLE" OR "${_type}" STREQUAL "GUI_EXECUTABLE")
            target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_MAIN)
        endif()
    else()
        if(BORLAND OR WATCOM)
            if("${_type}" STREQUAL "CONSOLE_EXECUTABLE")
                target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_MAIN)
            elseif("${_type}" STREQUAL "GUI_EXECUTABLE")
                target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_WINMAIN)
            endif()
        elseif(MSVC AND NOT OLD_MSVC)
            if("${_type}" STREQUAL "CONSOLE_EXECUTABLE")
                target_compile_definitions(${_target} PRIVATE
                    $<$<CONFIG:Debug>:RUNTIME_PROVIDE_MAIN>
                    $<$<CONFIG:Release>:RUNTIME_PROVIDE_CONMAIN>
                    $<$<CONFIG:RelWithDebInfo>:RUNTIME_PROVIDE_CONMAIN>
                    $<$<CONFIG:MinSizeRel>:RUNTIME_PROVIDE_CONMAIN>
                    )
            elseif("${_type}" STREQUAL "GUI_EXECUTABLE")
                target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_WINMAIN)
            endif()
        else()
            if("${_type}" STREQUAL "CONSOLE_EXECUTABLE")
                target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_CONMAIN)
            elseif("${_type}" STREQUAL "GUI_EXECUTABLE")
                target_compile_definitions(${_target} PRIVATE RUNTIME_PROVIDE_WINMAIN)
            endif()
        endif()
    endif()

    if(MSVC)
        set(nodebug NODEBUG)
        if(OLD_MSVC)
            set(nodebug)
            target_link_libraries(${_target} PRIVATE ${CRTDLL_LIB})
        endif()

        extra_linker_options(TARGET ${_target} ${nodebug} /NODEFAULTLIB)
        if(NOT OLD_MSVC)
            if(NOT "${_type}" STREQUAL "STATIC_LIBRARY")
                extra_linker_options(TARGET ${_target} /MANIFEST:NO /SAFESEH:NO /DYNAMICBASE:NO)
            endif()
        else()
            if("${_type}" STREQUAL "CONSOLE_EXECUTABLE")
                extra_linker_options(TARGET ${_target} /SUBSYSTEM:CONSOLE,3.10)
            elseif("${_type}" STREQUAL "GUI_EXECUTABLE")
                extra_linker_options(TARGET ${_target} /SUBSYSTEM:WINDOWS,3.10)
            endif()
        endif()

        if(CPU32)
            if(NOT OLD_MSVC)
                target_link_libraries(${_target} PRIVATE ${MSVCRT32})
            endif()

            if(NOT "${_type}" STREQUAL "STATIC_LIBRARY")
                if("${_type}" STREQUAL "DYNAMIC_LIBRARY")
                    extra_linker_options(TARGET ${_target} ${nodebug} /ENTRY:DllMain@12)
                else()
                    extra_linker_options(TARGET ${_target} ${nodebug} /ENTRY:WinMain@16)
                endif()
            endif()
        endif()

        if(CPU64)
            target_link_libraries(${_target} PRIVATE ${MSVCRT64})

            if(NOT "${_type}" STREQUAL "STATIC_LIBRARY")
                if("${_type}" STREQUAL "DYNAMIC_LIBRARY")
                    extra_linker_options(TARGET ${_target} ${nodebug} /ENTRY:DllMain)
                else()
                    extra_linker_options(TARGET ${_target} ${nodebug} /ENTRY:WinMain)
                endif()
            endif()
        endif()
    endif()

    if(MINGW)
        extra_linker_options(TARGET ${_target}
            -nostdlib
            -nostartfiles
            )

        if(CPU32)
            extra_linker_options(TARGET ${_target}
                -Wl,--major-os-version=3
                -Wl,--minor-os-version=10
                -Wl,--major-subsystem-version=3
                -Wl,--minor-subsystem-version=10
                "${CRTDLL_DLL}"
                )
            target_link_libraries(${_target} PRIVATE
                gcc
                "${CRTDLL_DLL}"
                )
            if(NOT "${_type}" STREQUAL "STATIC_LIBRARY")
                if("${_type}" STREQUAL "DYNAMIC_LIBRARY")
                    extra_linker_options(TARGET ${_target} -Wl,--entry,_DllMain@12)
                else()
                    extra_linker_options(TARGET ${_target}
                        -Wl,--pic-executable
                        -Wl,--emit-relocs
                        -Wl,--image-base=0x01000000
                        -Wl,--entry,_WinMain@16
                        )
                endif()
            endif()
        endif()

        if(CPU64)
            target_link_libraries(${_target} PRIVATE
                gcc
                msvcrt
                )
            if(NOT "${_type}" STREQUAL "STATIC_LIBRARY")
                if("${_type}" STREQUAL "DYNAMIC_LIBRARY")
                    extra_linker_options(TARGET ${_target} -Wl,--entry,DllMain@12)
                else()
                    extra_linker_options(TARGET ${_target} -Wl,--entry,WinMain@16)
                endif()
            endif()
        endif()
    endif()

    if(EMSCRIPTEN)
        extra_linker_options(TARGET ${_target}
            "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE='$UTF8ToString'"
            )
    endif()

endmacro()

#
# Adds tests to the target
#
function(_add_tests _target)
    foreach(_test ${ARGN})
        get_filename_component(_file "${_test}" ABSOLUTE)
        source_group("Tests" FILES "${_file}")
        target_sources(${_target} PRIVATE "${_file}")
        set_property(TARGET ${_target} APPEND PROPERTY _TESTS "${_test}")
    endforeach()
endfunction()

#
# Generate list of tests
#
macro(_collect_tests _target)
    list(FIND _visited "${_target}" _idx)
    if(_idx EQUAL -1)
        list(APPEND _visited "${_target}")

        get_property(_target_tests TARGET "${_target}" PROPERTY _TESTS)
        if(_target_tests)
            list(APPEND _tests ${_target_tests})
        endif()

        get_target_property(_target_libs "${_target}" LINK_LIBRARIES)
        if(_target_libs)
            foreach(_lib IN LISTS _target_libs)
                if(TARGET "${_lib}")
                    _collect_tests("${_lib}")
                endif()
            endforeach()
        endif()
    endif()
endmacro()

#
# Produce application library
#
#   create_library(name [STATIC] [SOURCES] sources [DEF file] [TESTS files])
#
function(create_library _target)

    set(_single DEF)
    set(_multi SOURCES TESTS)
    set(_options STATIC)
    cmake_parse_arguments(_opt "${_options}" "${_single}" "${_multi}" ${ARGN})

    if(STATIC_BUILD)
        set(_opt_STATIC TRUE)
    endif()

    string(TOUPPER "${_target}" _target_upper)
    string(REPLACE "-" "_" _target_upper "${_target_upper}")

    if(${_opt_STATIC})
        add_library(${_target} STATIC ${_opt_UNPARSED_ARGUMENTS} ${_opt_SOURCES})
        target_compile_definitions(${_target} PUBLIC ${_target_upper}_STATIC)
    else()
        add_library(${_target} SHARED ${_opt_UNPARSED_ARGUMENTS} ${_opt_SOURCES})
        set_target_properties(${_target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${OUTDIR}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTDIR}/debug"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTDIR}/release"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTDIR}/release.dbg"
            RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUTDIR}/release.min"
            LIBRARY_OUTPUT_DIRECTORY "${OUTDIR}"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTDIR}/debug"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTDIR}/release"
            LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTDIR}/release.dbg"
            LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${OUTDIR}/release.min"
            SKIP_BUILD_RPATH FALSE
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "$ORIGIN"
            )
        target_compile_definitions(${_target} PRIVATE ${_target_upper}_EXPORTS)
    endif()

    if(_opt_DEF)
        source_group("" FILES "${_opt_DEF}")
        target_sources(${_target} PRIVATE "${_opt_DEF}")
        if(NOT ${_opt_STATIC})
            get_filename_component(_def "${_opt_DEF}" ABSOLUTE)
            if(BORLAND)
                set_target_properties(${_target} PROPERTIES LINK_FLAGS " ${_def} ")
            endif()
        else()
            set_source_files_properties("${_opt_DEF}" PROPERTIES HEADER_FILE_ONLY TRUE)
        endif()
    endif()

    if(_opt_TESTS)
        _add_tests(${_target} ${_opt_TESTS})
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

    _apply_compiler_flags(${_target})
    if(${_opt_STATIC})
        _apply_linker_flags(${_target} STATIC_LIBRARY)
    else()
        _apply_linker_flags(${_target} DYNAMIC_LIBRARY)
    endif()

endfunction()

#
# Produce executable
#
#   create_executable(name [GUI|CONSOLE] [TESTING] [SOURCES] sources [TESTS tests])
#
function(create_executable _target)

    set(_single)
    set(_multi SOURCES TESTS)
    set(_options GUI CONSOLE TESTING)
    cmake_parse_arguments(_opt "${_options}" "${_single}" "${_multi}" ${ARGN})

    if(_opt_GUI OR _opt_CONSOLE)
        if(_opt_GUI AND _opt_CONSOLE)
            message(FATAL_ERROR "create_executable: GUI and CONSOLE are mutually exclusive.")
        endif()
    endif()

    set(_src ${_opt_SOURCES} ${_opt_UNPARSED_ARGUMENTS})
    if(_opt_TESTING)
        set(_tests_c "${CMAKE_CURRENT_BINARY_DIR}/${_target}.c")
        source_group("Generated Files" FILES "${_tests_c}")
        list(APPEND _src "${_tests_c}")
    endif()

    if(NOT _opt_CONSOLE)
        add_executable(${_target} WIN32 MACOSX_BUNDLE ${_src})
        _apply_linker_flags(${_target} GUI_EXECUTABLE)
        set(_id "APP_GUI")
    else()
        add_executable(${_target} ${_src})
        _apply_linker_flags(${_target} CONSOLE_EXECUTABLE)
        set(_id "APP_CONSOLE")
    endif()

    _apply_compiler_flags(${_target})
    target_link_libraries(${_target} PRIVATE runtime)

    set_target_properties(${_target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTDIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTDIR}/debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTDIR}/release"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTDIR}/release.dbg"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUTDIR}/release.min"
        SKIP_BUILD_RPATH FALSE
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN"
        )

    if(_opt_TESTS)
        if(NOT _opt_TESTING)
            message(FATAL_ERROR "create_executable: TESTS without TESTING are useless.")
        endif()
        _add_tests(${_target} ${_opt_TESTS})
    endif()

    if(_opt_TESTING)
        target_link_libraries(${_target} PRIVATE testing)

        set(_visited)
        set(_tests)
        _collect_tests("${_target}")

        set(_testcode "#include <runtime/main.h>\n")
        set(_testcode "${_testcode}#include <runtime/testing/utility.h>\n")
        set(_testcode "${_testcode}\n")
        foreach(_test ${_tests})
            set(_testcode "${_testcode}#include <${_test}>\n")
        endforeach()
        set(_testcode "${_testcode}\n")
        set(_testcode "${_testcode}static const Test tests[] = \{\n")
        foreach(_test ${_tests})
            get_filename_component(_name "${_test}" NAME_WE)
            set(_testcode "${_testcode}    \{ \"${_name}\", test_${_name} \},\n")
        endforeach()
        set(_testcode "${_testcode}    \{ NULL, NULL \},\n")
        set(_testcode "${_testcode}\};\n")
        set(_testcode "${_testcode}\n")
        set(_testcode "${_testcode}int AppMain(int argc, char** argv)\n")
        set(_testcode "${_testcode}\{\n")
        set(_testcode "${_testcode}    return run_tests(argc, argv, ${_id}, tests);\n")
        set(_testcode "${_testcode}\}\n")
        smart_write_file("${_tests_c}" "${_testcode}")
    endif()

endfunction()

######################################################################################################################

if(NOT "${PROJECT_NAME}" STREQUAL "Runtime")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../.." runtime)
endif()
