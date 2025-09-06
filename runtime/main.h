#ifndef RUNTIME_MAIN_H
#define RUNTIME_MAIN_H

#include <runtime/platform.h>

EXTERN_C_BEGIN

/* provided by the app code */
typedef int (*PFN_AppMain)(int, char**);
int AppMain(int argc, char** argv);

RUNTIME_API int RuntimeMain(RuntimeVersion version, PFN_AppMain appMain, int argc, char** argv);

#ifdef RUNTIME_PLATFORM_MSWIN
RUNTIME_API int RuntimeWinMain(RuntimeVersion version, PFN_AppMain appMain,
    void* hInstance, void* hPrevInstance, const char* cmdLine, int nShowCmd);
#endif

EXTERN_C_END

/* At least Watcom 10.0 is unable to fetch `main` or `WinMain` from a static library. So we use a combination
 * of define in `DrunkFly/Runtime.cmake` and macros here to provide entry point and redirect to the runtime
 * library initialization routines.
 */

#ifdef RUNTIME_PROVIDE_MAIN
 #define AppMain \
    AppMain(int argc, char** argv); \
    int main(int argc, char** argv) { \
        return RuntimeMain(RUNTIME_VERSION_CURRENT, AppMain, argc, argv); \
    } \
    int AppMain
#endif

#ifdef RUNTIME_PROVIDE_WINMAIN
 #include <runtime/mswin/winapi.h>
 #define AppMain \
    AppMain(int argc, char** argv); \
    int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) { \
        return RuntimeWinMain(RUNTIME_VERSION_CURRENT, AppMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd); \
    } \
    int AppMain
#endif

#endif
