#ifndef RUNTIME_MSWIN_WINAPI_H
#define RUNTIME_MSWIN_WINAPI_H

#include <runtime/platform.h>

/****************************************************************************/

/* disable warning causing trouble at each GetProcAddress site */
#ifdef _MSC_VER
 #pragma warning(disable:4191) /* type cast: unsafe conversion from FARPROC to ... */
#endif
#if defined(__GNUC__) && __GNUC__ >= 8
 #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

/* some warnings happen inside <windows.h> */
#ifdef _MSC_VER
 #if _MSC_VER >= 1200
  #pragma warning(push)
 #endif
 #if _MSC_VER >= 1912
  #pragma warning(disable:5039) /* potentially throwing function passed to 'extern "C"' function */
 #endif
 #pragma warning(disable:4201) /* nonstandard extension used : nameless struct/union */
 #pragma warning(disable:4214) /* nonstandard extension used : bit field types other than int */
#endif

/****************************************************************************/

#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
 #define NOMINMAX
#endif

#ifdef RUNTIME_PLATFORM_MSWIN_WIN64
 /* 64-bit platform is guaranteed to support Unicode APIs */
 #ifndef UNICODE
  #define UNICODE
 #endif
 #ifndef _UNICODE
  #define _UNICODE
 #endif
#else
 /* On 32-bit platforms we keep compatibility up to Win32s which require ANSI APIs */
 #ifdef UNICODE
  #undef UNICODE
 #endif
 #ifdef _UNICODE
  #undef _UNICODE
 #endif
#endif

#include <windows.h>

/****************************************************************************/

EXTERN_C_BEGIN

typedef LPWSTR* (WINAPI* PFN_CommandLineToArgvW)(LPCWSTR, int*);
typedef BOOL (WINAPI* PFN_DisableThreadLibraryCalls)(HMODULE);
typedef LPWSTR (WINAPI* PFN_GetCommandLineW)(void);
typedef DWORD (WINAPI* PFN_GetModuleFileNameW)(HMODULE, LPWSTR, DWORD);

FARPROC WinGetProcAddress_(HANDLE* hDll, const TCHAR* dll, const char* proc);
#define WinGetProcAddress(MODULE, PROC) ((PFN_##PROC)WinGetProcAddress_(&g_h##MODULE, TEXT(#MODULE), #PROC))

#define EXTPROCDECL(PROC) PFN_##PROC pfn##PROC
#define EXTPROCLOAD(MODULE, PROC) pfn##PROC = WinGetProcAddress(MODULE, PROC)
#define EXTPROC(MODULE, PROC) EXTPROCDECL(PROC) = WinGetProcAddress(MODULE, PROC)

extern BOOL g_isGuiProgram;
extern HINSTANCE g_hInstance;

#if !defined(RUNTIME_PLATFORM_MSWIN_WIN16)
extern HANDLE g_hKernel32;
extern HANDLE g_hShell32;
extern HANDLE g_hProcessHeap;
#endif

EXTERN_C_END

/****************************************************************************/

#if defined(_MSC_VER) && _MSC_VER >= 1200
 #pragma warning(pop)
#endif

/****************************************************************************/

#endif
