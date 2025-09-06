@"%~dp0tools\pour_wrapper_windows.exe" --script "%0" %* && exit /B 0 || exit /B 1

pour.build("msdos:djgpp_12.2.0:debug")
pour.build("msdos:djgpp_12.2.0:release")
pour.build("msdos:djgpp_12.2.0:relwithdebinfo")
pour.build("msdos:djgpp_12.2.0:minsizerel")

pour.require("vm-dos")

dosbox.prepend_autoexec_bat("mount w: "..SCRIPT_DIR.."/build-artifacts/msdos")
dosbox.autoexec_bat("W:\\djgpp122\\debug\\testcon.exe")
dosbox.autoexec_bat("W:\\djgpp122\\debug\\testgui.exe")
dosbox.autoexec_bat("W:\\djgpp122\\release\\testcon.exe")
dosbox.autoexec_bat("W:\\djgpp122\\release\\testgui.exe")
dosbox.autoexec_bat("W:\\djgpp122\\release.dbg\\testcon.exe")
dosbox.autoexec_bat("W:\\djgpp122\\release.dbg\\testgui.exe")
dosbox.autoexec_bat("W:\\djgpp122\\release.min\\testcon.exe")
dosbox.autoexec_bat("W:\\djgpp122\\release.min\\testgui.exe")

dosbox.run()
