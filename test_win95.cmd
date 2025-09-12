@"%~dp0tools\pour_wrapper_windows.exe" --script "%0" %* && exit /B 0 || exit /B 1

pour.build("win32:borland_4.5.2:debug")
pour.build("win32:borland_4.5.2:release")
pour.build("win32:borland_4.5.2:relwithdebinfo")
pour.build("win32:borland_4.5.2:minsizerel")
pour.build("win32:clang_4.0.0:debug")
pour.build("win32:clang_4.0.0:release")
pour.build("win32:clang_4.0.0:relwithdebinfo")
pour.build("win32:clang_4.0.0:minsizerel")
pour.build("win32:mingw_4.4.0:debug")
pour.build("win32:mingw_4.4.0:release")
pour.build("win32:mingw_4.4.0:relwithdebinfo")
pour.build("win32:mingw_4.4.0:minsizerel")
pour.build("win32:mingw_8.1.0:debug")
pour.build("win32:mingw_8.1.0:release")
pour.build("win32:mingw_8.1.0:relwithdebinfo")
pour.build("win32:mingw_8.1.0:minsizerel")
pour.build("win32:msvc_2.0:debug")
pour.build("win32:msvc_2.0:release")
pour.build("win32:msvc_2.0:relwithdebinfo")
pour.build("win32:msvc_2.0:minsizerel")
pour.build("win32:msvc_4.1:debug")
pour.build("win32:msvc_4.1:release")
pour.build("win32:msvc_4.1:relwithdebinfo")
pour.build("win32:msvc_4.1:minsizerel")
pour.build("win32:watcom_10.0a:debug")
pour.build("win32:watcom_10.0a:release")
pour.build("win32:watcom_10.0a:relwithdebinfo")
pour.build("win32:watcom_10.0a:minsizerel")

arg.silent = true
pour.require("vm-windows95")

local disk, DIR = mkdisk.get_disk("C:")
disk:make_directory(DIR["/"], "APP")
disk:add_directory(DIR["/"]["APP"], "build-artifacts/win32")
disk:add_directory(DIR["/"]["APP"], "runtime", "flat")
disk:add_directory(DIR["/"]["APP"], "tests", "flat")
disk:add_directory(DIR["/"]["APP"], "lib/stb", "flat")
disk:add_file(DIR["/"]["APP"], "testcon.c", "build/win32-borland_4.5.2-debug/testcon.c")
disk:add_file(DIR["/"]["APP"], "testgui.c", "build/win32-borland_4.5.2-debug/testgui.c")

disk:add_file_content(DIR["/"]["WINDOWS"]["Start Menu"]["Programs"]["StartUp"], "autorun.bat",
    "@echo off\r\n"..
    "C:\\App\\borland4\\debug\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\borland4\\debug\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\borland4\\release\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\borland4\\release\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\borland4\\release.dbg\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\borland4\\release.dbg\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\borland4\\release.min\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\borland4\\release.min\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\clang400\\debug\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\clang400\\debug\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\clang400\\release\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\clang400\\release\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\clang400\\release.dbg\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\clang400\\release.dbg\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\clang400\\release.min\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\clang400\\release.min\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw440\\debug\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw440\\debug\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw440\\release\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw440\\release\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw440\\release.dbg\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw440\\release.dbg\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw440\\release.min\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw440\\release.min\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw810\\debug\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw810\\debug\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw810\\release\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw810\\release\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw810\\release.dbg\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw810\\release.dbg\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\mingw810\\release.min\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\mingw810\\release.min\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    -- "C:\\App\\msvc20\\debug\\testcon\r\n"..
    -- "@if errorlevel 1 goto fail\r\n"..
    -- "start /wait C:\\App\\msvc20\\debug\\testgui\r\n"..
    -- "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\msvc20\\release\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\msvc20\\release\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\msvc20\\release.dbg\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\msvc20\\release.dbg\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\msvc20\\release.min\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\msvc20\\release.min\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    -- "C:\\App\\msvc41\\debug\\testcon\r\n"..
    -- "@if errorlevel 1 goto fail\r\n"..
    -- "start /wait C:\\App\\msvc41\\debug\\testgui\r\n"..
    -- "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\msvc41\\release\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\msvc41\\release\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\msvc41\\release.dbg\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\msvc41\\release.dbg\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\msvc41\\release.min\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\msvc41\\release.min\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\watcom10\\debug\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\watcom10\\debug\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\watcom10\\release\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\watcom10\\release\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\watcom10\\release.dbg\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\watcom10\\release.dbg\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "C:\\App\\watcom10\\release.min\\testcon\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "start /wait C:\\App\\watcom10\\release.min\\testgui\r\n"..
    "@if errorlevel 1 goto fail\r\n"..
    "@echo Done!\r\n"..
    "@pause\r\n"..
    "@exit\r\n"..
    ":fail\r\n"..
    "@echo **** FAIL! ****\r\n"..
    "@pause\r\n"
    )

dosbox.run()
