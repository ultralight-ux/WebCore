///
/// Copyright (C) 2022 Ultralight, Inc. All rights reserved.
///
/// Portions of the following source code is under the following license:
///
///   Copyright 2016 The Chromium Authors. All rights reserved.
///   Use of this source code is governed by a BSD-style license that can be
///   found in the LICENSE file.
///
/// This header defines symbols to override the functions in the MSVC UCRT.
///

#pragma once

#if defined(ULTRALIGHT_ENABLE_ALLOCATOR_OVERRIDE) && defined(_WIN32)

#include <malloc.h>
#include <new>
#include <windows.h>
#include <Ultralight/private/util/Debug.h>

extern "C" {

// These symbols override the CRT's implementation of the same functions.
__declspec(restrict) void* malloc(size_t size) { return ShimMalloc(size); }

__declspec(noinline) void __cdecl free(_Pre_maybenull_ _Post_invalid_ void* ptr) { ShimFree(ptr); }

__declspec(restrict) void* realloc(void* ptr, size_t size) { return ShimRealloc(ptr, size); }

__declspec(restrict) void* calloc(size_t n, size_t size) { return ShimCalloc(n, size); }

// _msize() is the Windows equivalent of malloc_size().
size_t _msize(void* memblock) { return ShimGetSizeEstimate(memblock); }

__declspec(restrict) void* _aligned_malloc(size_t size, size_t alignment) {
  return ShimAlignedMalloc(size, alignment);
}

__declspec(restrict) void* _aligned_realloc(void* address, size_t size, size_t alignment) {
  return ShimAlignedRealloc(address, size, alignment);
}

void _aligned_free(void* address) { ShimAlignedFree(address); }

// _recalloc_base is called by CRT internally.
__declspec(restrict) void* _recalloc_base(void* block, size_t count, size_t size) {
  const size_t old_block_size = (block != nullptr) ? _msize(block) : 0;
  size_t new_block_size = count;
  new_block_size *= size;

  void* const new_block = realloc(block, new_block_size);

  if (new_block != nullptr && old_block_size < new_block_size) {
    memset(static_cast<char*>(new_block) + old_block_size, 0, new_block_size - old_block_size);
  }

  return new_block;
}

__declspec(restrict) void* _malloc_base(size_t size) { return malloc(size); }

__declspec(restrict) void* _calloc_base(size_t n, size_t size) { return calloc(n, size); }

void _free_base(void* block) { free(block); }

__declspec(restrict) void* _recalloc(void* block, size_t count, size_t size) {
  return _recalloc_base(block, count, size);
}

// The following uncommon _aligned_* routines are not currently used and have
// been shimmed to immediately crash to ensure that implementations are added if
// uses are introduced.
__declspec(restrict) void* _aligned_recalloc(void* address, size_t num, size_t size,
                                             size_t alignment) {
  UL_NOTIMPLEMENTED();
  exit(-1);
}

size_t _aligned_msize(void* address, size_t alignment, size_t offset) {
  UL_NOTIMPLEMENTED();
  exit(-1);
}

__declspec(restrict) void* _aligned_offset_malloc(size_t size, size_t alignment, size_t offset) {
  UL_NOTIMPLEMENTED();
  exit(-1);
}

__declspec(restrict) void* _aligned_offset_realloc(void* address, size_t size, size_t alignment,
                                                   size_t offset) {
  UL_NOTIMPLEMENTED();
  exit(-1);
}

__declspec(restrict) void* _aligned_offset_recalloc(void* address, size_t num, size_t size,
                                                    size_t alignment, size_t offset) {
  UL_NOTIMPLEMENTED();
  exit(-1);
}

} // extern "C"

#endif