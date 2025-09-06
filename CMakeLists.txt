cmake_minimum_required(VERSION 3.5.2...3.31.4)
project(Runtime)

include(DrunkFly/Runtime)

create_source_list(src runtime
    HTML5_SOURCES
        html5/webmain.c
    LINUX_SOURCES
        linux/linmain.c
    MSDOS_SOURCES
        msdos/dosmain.c
    MSWIN_SOURCES
        mswin/winapi.c
        mswin/winapi.h
        mswin/winmain.c
    SOURCES
        detail/endian.h
        detail/types.h
        common.h
        main.h
        mem.c
        mem.h
        platform.h
        version.h
    )

create_source_list(src_main main
    HTML5_SOURCES
        html5.c
    MSWIN_SOURCES
        wincon.c
        wingui.c
    )

safe_force_c89()
enable_maximum_warnings()

create_library(runtime ${src})

create_console_executable(hello tests/hello.c)
create_gui_executable(hellogui tests/hellogui.c)
