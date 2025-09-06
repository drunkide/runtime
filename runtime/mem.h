#ifndef RUNTIME_MEM_H
#define RUNTIME_MEM_H

#include <runtime/common.h>

EXTERN_C_BEGIN

RUNTIME_API void* MemAlloc(size_t size);
RUNTIME_API void* MemZeroAlloc(size_t size);
RUNTIME_API void* MemRealloc(void* ptr, size_t size);
RUNTIME_API void MemFree(void* ptr);

EXTERN_C_END

#endif
