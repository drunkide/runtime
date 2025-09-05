cmake_minimum_required(VERSION 3.5.2...3.31.4)
project(Runtime)

include(DrunkFly/Runtime)

if(NOT CPU64)
    force_c89()
endif()

enable_maximum_warnings()

my_add_library(runtime
    detail/endian.h
    detail/types.h
    common.h
    mem.c
    mem.h
    win32.h
    )
