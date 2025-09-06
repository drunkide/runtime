@"%~dp0..\..\tools\pour_wrapper_windows.exe" --chdir "%~dp0." --script "%0" %* && exit /B 0 || exit /B 1
pour.run("vm-windows-nt31:msvc-lib", "/nologo", "/def:crtdll.def", "/machine:IX86", "/out:crtdll.lib")
pour.run("mingw32-8.1.0:dlltool", "-d", "msvcrt.def", "-l", "msvcrt32.lib")
pour.run("mingw64-8.1.0:dlltool", "-d", "msvcrt.def", "-l", "msvcrt64.lib")
