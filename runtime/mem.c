#include <runtime/mem.h>
#include <runtime/platform.h>

#ifdef RUNTIME_PLATFORM_MSWIN_WIN32
 #include <runtime/mswin/winapi.h>
#else
 #include <stdlib.h>
#endif

void* MemAlloc(size_t size)
{
    if (size == 0)
        size = 1;

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN32
    return HeapAlloc(g_hProcessHeap, 0, size);
  #else
    return malloc(size);
  #endif
}

void* MemZeroAlloc(size_t size)
{
    if (size == 0)
        size = 1;

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN32
    return HeapAlloc(g_hProcessHeap, HEAP_ZERO_MEMORY, size);
  #else
    return calloc(1, size);
  #endif
}

void* MemRealloc(void* ptr, size_t size)
{
    if (size == 0)
        size = 1;

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN32
    if (!ptr)
        return HeapAlloc(g_hProcessHeap, 0, size);
    return HeapReAlloc(g_hProcessHeap, 0, ptr, size);
  #else
    return realloc(ptr, size);
  #endif
}

void MemFree(void* ptr)
{
    if (!ptr)
        return;

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN32
    HeapFree(g_hProcessHeap, 0, ptr);
  #else
    free(ptr);
  #endif
}
