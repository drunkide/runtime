
table_append(CMAKE_MODULE_PATH, SCRIPT_DIR..'/cmake')

target 'win32:borland_4.5.2'
target 'win32:clang_4.0.0'
target 'win32:mingw_4.4.0'
target 'win32:mingw_8.1.0'
target 'win32:msvc_2.0'
target 'win32:msvc_4.1'
target 'win32:msvc_2022'
target 'win32:watcom_10.0a'

target 'win64:mingw_8.1.0'
target 'win64:msvc_2022'

target 'linux_x86:egcs_1.1.2'

target 'linux_x64:clang_3.5.0'
