#ifndef RUNTIME_MEM_H
#define RUNTIME_MEM_H

#include <runtime/common.h>

void* MemAlloc(size_t size);
void* MemZeroAlloc(size_t size);
void* MemRealloc(void* ptr, size_t size);
void MemFree(void* ptr);

#endif
