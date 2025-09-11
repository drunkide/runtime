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

create_source_list(src_tests tests
    GROUP
        Tests
    SOURCES
        cases/sprintf.h
        tests.h
    )

safe_force_c89()
enable_maximum_warnings()

create_library(runtime ${src} ${src_tests})

if(MSVC)
    add_library(fltused STATIC runtime/detail/fltused.c)
    target_link_libraries(runtime PUBLIC fltused)
endif()

create_gui_executable(testgui tests/testgui.c)
create_console_executable(testcon tests/testcon.c)

set_target_properties(testgui PROPERTIES FOLDER "Tests")
set_target_properties(testcon PROPERTIES FOLDER "Tests")
