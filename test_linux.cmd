@"%~dp0tools\pour_wrapper_windows.exe" --script "%0" console=true %* && exit /B 0 || exit /B 1

pour.build("linux_x86:egcs_1.1.2:debug")
pour.build("linux_x86:egcs_1.1.2:release")
pour.build("linux_x86:egcs_1.1.2:relwithdebinfo")
pour.build("linux_x86:egcs_1.1.2:minsizerel")

arg.silent = true
pour.require("vm-basiclinux35")

local disk, DIR = mkdisk.get_disk("/")
disk:make_directory(DIR["/"], "App")
disk:add_directory(DIR["/"]["App"], "build-artifacts/linux32")

disk:add_file_content(DIR["/"]["tmp"], "console_autorun.sh",
    "LD_LIBRARY_PATH=/App/egcs112/debug:$LD_LIBRARY_PATH /App/egcs112/debug/testcon\n"..
    "LD_LIBRARY_PATH=/App/egcs112/debug:$LD_LIBRARY_PATH /App/egcs112/debug/testgui\n"..
    "LD_LIBRARY_PATH=/App/egcs112/release:$LD_LIBRARY_PATH /App/egcs112/release/testcon\n"..
    "LD_LIBRARY_PATH=/App/egcs112/release:$LD_LIBRARY_PATH /App/egcs112/release/testgui\n"..
    "LD_LIBRARY_PATH=/App/egcs112/release.dbg:$LD_LIBRARY_PATH /App/egcs112/release.dbg/testcon\n"..
    "LD_LIBRARY_PATH=/App/egcs112/release.dbg:$LD_LIBRARY_PATH /App/egcs112/release.dbg/testgui\n"..
    "LD_LIBRARY_PATH=/App/egcs112/release.min:$LD_LIBRARY_PATH /App/egcs112/release.min/testcon\n"..
    "LD_LIBRARY_PATH=/App/egcs112/release.min:$LD_LIBRARY_PATH /App/egcs112/release.min/testgui\n"
    )

dosbox.run()
