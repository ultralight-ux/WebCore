///
/// Copyright (C) 2022 Ultralight, Inc. All rights reserved.
///
/// Portions of the following source code is under the following license:
///
///   Copyright 2016 The Chromium Authors. All rights reserved.
///   Use of this source code is governed by a BSD-style license that can be
///   found in the LICENSE file.
///

#if defined(ULTRALIGHT_MODULE_WEBCORE)
#include "config.h"
#endif
#include "MemoryProfiler.h"

#if defined(ULTRALIGHT_ENABLE_MEMORY_PROFILER) && defined(_WIN32)

#include "MemoryProfiler_WinHeap.h"
#include <string.h>

#define __THROW noexcept
#define SHIM_ALWAYS_EXPORT __declspec(noinline)

#if defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
#define COMPILER_GCC 1
#elif defined(_MSC_VER)
#define COMPILER_MSVC 1
#endif

#if defined(COMPILER_GCC) && defined(NDEBUG)
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#elif defined(COMPILER_MSVC) && defined(NDEBUG)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif

bool g_call_new_handler_on_malloc_failure = false;

ALWAYS_INLINE size_t GetCachedPageSize() {
  // System pagesize. This value remains constant on x86/64 architectures.
  constexpr int PAGESIZE_KB = 4;
  return PAGESIZE_KB * 1024;
}

// Calls the std::new handler thread-safely. Returns true if a new_handler was
// set and called, false if no new_handler was set.
bool CallNewHandler(size_t size) { return WinCallNewHandler(size); }

// When true makes malloc behave like new, w.r.t calling the new_handler if
// the allocation fails (see set_new_mode() in Windows).
void SetCallNewHandlerOnMallocFailure(bool value) { g_call_new_handler_on_malloc_failure = value; }

// Allocates |size| bytes or returns nullptr. It does NOT call the new_handler,
// regardless of SetCallNewHandlerOnMallocFailure().
void* UncheckedAlloc(size_t size) { return WinHeapMalloc(size); }

// Frees memory allocated with UncheckedAlloc().
void UncheckedFree(void* ptr) { WinHeapFree(ptr); }

extern "C" {

ALWAYS_INLINE void* ShimCppNew(size_t size) {
  void* ptr;
  void* context = nullptr;
  ptr = WinHeapMalloc(size);
  return ptr;
}

ALWAYS_INLINE void* ShimCppNewNoThrow(size_t size) { return WinHeapMalloc(size); }

ALWAYS_INLINE void* ShimCppAlignedNew(size_t size, size_t alignment) {
  void* ptr;
  void* context = nullptr;
  ptr = WinHeapAlignedMalloc(size, alignment);
  return ptr;
}

ALWAYS_INLINE void ShimCppDelete(void* address) { return WinHeapFree(address); }

ALWAYS_INLINE void* ShimMalloc(size_t size, void* context) {
  void* ptr;
  ptr = WinHeapMalloc(size);
  return ptr;
}

ALWAYS_INLINE void* ShimCalloc(size_t n, size_t elem_size, void* context) {
  // Overflow check.
  const size_t size = n * elem_size;
  if (elem_size != 0 && size / elem_size != n)
    return nullptr;

  void* ptr;
  ptr = WinHeapMalloc(size);
  if (ptr) {
    memset(ptr, 0, size);
  }
  return ptr;
}

ALWAYS_INLINE void* ShimRealloc(void* address, size_t size, void* context) {
  void* ptr;
  ptr = WinHeapRealloc(address, size);
  return ptr;
}

ALWAYS_INLINE void ShimFree(void* address, void* context) { WinHeapFree(address); }

ALWAYS_INLINE size_t ShimGetSizeEstimate(const void* address, void* context) {
  return WinHeapGetSizeEstimate(const_cast<void*>(address));
}

ALWAYS_INLINE void* ShimAlignedMalloc(size_t size, size_t alignment, void* context) {
  void* ptr;
  ptr = WinHeapAlignedMalloc(size, alignment);
  return ptr;
}

ALWAYS_INLINE void* ShimAlignedRealloc(void* address, size_t size, size_t alignment,
                                       void* context) {
  void* ptr;
  ptr = WinHeapAlignedRealloc(address, size, alignment);
  return ptr;
}

ALWAYS_INLINE void ShimAlignedFree(void* address, void* context) { WinHeapAlignedFree(address); }

} // extern "C"

#include "MemoryProfiler_CppSymbols.h"

#if defined(_WIN32)
#include "MemoryProfiler_Windows.h"
#endif

#endif