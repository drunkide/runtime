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
arg.color = '16'
pour.require("vm-windows311")

dosbox.prepend_autoexec_bat("mount w: "..SCRIPT_DIR.."/build-artifacts/win32")

-- FIXME
local grp = wingrp.get("StartUp")
grp:add(0, 0, "Test", "W:\\mingw440\\debug\\testgui.exe")

dosbox.run()
