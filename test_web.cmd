@"%~dp0tools\pour_wrapper_windows.exe" --script "%0" %* && exit /B 0 || exit /B 1

pour.build("html5:emsdk_4.0.14:debug")
pour.build("html5:emsdk_4.0.14:release")
pour.build("html5:emsdk_4.0.14:relwithdebinfo")
pour.build("html5:emsdk_4.0.14:minsizerel")

-- start web server
pour.run_background("python3",
    "-m", "http.server",
    "-d", SCRIPT_DIR.."/build-artifacts/html5",
    "8198")

pour.exec("C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe",
    "--user-data-dir="..SCRIPT_DIR.."/build/html5-emsdk_4.0.14-debug/.msedge",
    "--no-first-run",
    "--no-default-browser-check",
    "http://localhost:8198/debug/testcon.html",
    "http://localhost:8198/release/testcon.html"
    )

-- end web server
pour.terminate_background_app()
