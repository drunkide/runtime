cmake_minimum_required(VERSION 3.5.2...3.31.4)
project(Runtime)

include(DrunkFly/Runtime)

create_source_list(src runtime
    HTML5_SOURCES
        html5/weberror.c
        html5/webmain.c
    LINUX_SOURCES
        linux/linerror.c
        linux/linmain.c
    MSDOS_SOURCES
        msdos/doserror.c
        msdos/dosmain.c
    MSWIN_SOURCES
        mswin/winapi.c
        mswin/winapi.h
        mswin/winerror.c
        mswin/winmain.c
    SOURCES
        detail/endian.h
        detail/int64.c
        detail/int64.h
        detail/types.h
        common.h
        error.c
        error.h
        main.h
        mem.c
        mem.h
        platform.h
        string.c
        string.h
        version.h
    )

create_source_list(src_testing .
    SOURCES
        runtime/testing/utility.c
        runtime/testing/utility.h
    )

safe_force_c89()
enable_maximum_warnings()

create_library(runtime ${src}
    TESTS
        tests/sprintf.h
    )

add_library(testing STATIC ${src_testing})
set_target_properties(testing PROPERTIES FOLDER "Tests")
target_link_libraries(testing PUBLIC runtime)

if(MSVC)
    add_library(msvc_fltused STATIC runtime/detail/fltused.c)
    set_target_properties(msvc_fltused PROPERTIES FOLDER "CMake")
    target_link_libraries(runtime PUBLIC msvc_fltused)
endif()

create_executable(testgui TESTING GUI)
create_executable(testcon TESTING CONSOLE)

set_target_properties(testgui PROPERTIES FOLDER "Tests")
set_target_properties(testcon PROPERTIES FOLDER "Tests")
