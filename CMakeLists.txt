cmake_minimum_required(VERSION 3.5.2...3.31.4)
project(Runtime)

include(DrunkFly/Runtime)

safe_force_c89()
enable_maximum_warnings()

create_source_list(src runtime
    HTML5_SOURCES
        html5/weblog.c
        html5/webmain.c
    LINUX_SOURCES
        linux/linlog.c
        linux/linmain.c
    MSDOS_SOURCES
        msdos/doslog.c
        msdos/dosmain.c
    MSWIN_SOURCES
        mswin/winapi.c
        mswin/winapi.h
        mswin/windisp.c
        mswin/windisp.h
        mswin/winlog.c
        mswin/winlog.h
        mswin/winmain.c
        mswin/winutil.c
        mswin/winutil.h
    SOURCES
        detail/endian.h
        detail/int64.c
        detail/int64.h
        detail/types.h
        buffer.c
        buffer.h
        common.h
        log.c
        log.h
        main.h
        mem.c
        mem.h
        platform.h
        string.c
        string.h
        version.h
    )

create_source_list(src_lib lib
    GROUP "Libraries"
    HEADERS
        crtdll/crtdll.def
        crtdll/msvcrt.def
    SOURCES
        stb/sprintf.h
        utf8/utf8.c
        utf8/utf8.h
    )

create_source_list(src_testing .
    SOURCES
        runtime/testing/utility.c
        runtime/testing/utility.h
    )

create_library(runtime
    SOURCES
        ${src}
        ${src_lib}
    TESTS
        tests/arith64.h
        tests/strfmt.h
        tests/unicode.h
        tests/utf8str.h
    )

if(MSVC)
    add_library(msvc_fltused STATIC runtime/detail/fltused.c)
    set_target_properties(msvc_fltused PROPERTIES FOLDER "CMake")
    target_link_libraries(runtime PUBLIC msvc_fltused)
endif()

create_library(testing STATIC ${src_testing})
set_target_properties(testing PROPERTIES FOLDER "Testing")

create_executable(testgui TESTING GUI)
create_executable(testcon TESTING CONSOLE)

set_target_properties(testgui PROPERTIES FOLDER "Testing")
set_target_properties(testcon PROPERTIES FOLDER "Testing")
