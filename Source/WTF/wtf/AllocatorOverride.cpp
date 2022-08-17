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

#if defined(ULTRALIGHT_ENABLE_ALLOCATOR_OVERRIDE) && defined(_WIN32)

#include <Ultralight/platform/Allocator.h>
#include <string.h>

#define __THROW noexcept

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

extern "C" {

ALWAYS_INLINE void* ShimCppNew(size_t size) { return ulAllocator.malloc(size); }

ALWAYS_INLINE void* ShimCppNewNoThrow(size_t size) { return ulAllocator.malloc(size); }

ALWAYS_INLINE void* ShimCppAlignedNew(size_t size, size_t alignment) {
  return ulAllocator.aligned_malloc(size, alignment);
}

ALWAYS_INLINE void ShimCppDelete(void* address) { return ulAllocator.free(address); }

ALWAYS_INLINE void* ShimMalloc(size_t size) { return ulAllocator.malloc(size); }

ALWAYS_INLINE void* ShimCalloc(size_t n, size_t elem_size) {
  // Overflow check.
  const size_t size = n * elem_size;
  if (elem_size != 0 && size / elem_size != n)
    return nullptr;

  void* ptr = ulAllocator.malloc(size);

  if (ptr)
    memset(ptr, 0, size);

  return ptr;
}

ALWAYS_INLINE void* ShimRealloc(void* address, size_t size) {
  return ulAllocator.realloc(address, size);
}

ALWAYS_INLINE void ShimFree(void* address) { ulAllocator.free(address); }

ALWAYS_INLINE size_t ShimGetSizeEstimate(const void* address) {
  return ulAllocator.get_size_estimate(const_cast<void*>(address));
}

ALWAYS_INLINE void* ShimAlignedMalloc(size_t size, size_t alignment) {
  return ulAllocator.aligned_malloc(size, alignment);
}

ALWAYS_INLINE void* ShimAlignedRealloc(void* address, size_t size, size_t alignment) {
  return ulAllocator.aligned_realloc(address, size, alignment);
}

ALWAYS_INLINE void ShimAlignedFree(void* address) { ulAllocator.aligned_free(address); }

} // extern "C"

#if defined(_WIN32)
#include "AllocatorOverride_Windows.h"
#endif

#endif
