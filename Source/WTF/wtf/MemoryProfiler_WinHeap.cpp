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
#include "MemoryProfiler_WinHeap.h"
#include "MemoryProfiler.h"

#if (defined(TRACY_PROFILE_MEMORY) || defined(ULTRALIGHT_ENABLE_MEMORY_STATS)) && defined(_WIN32)

#include <limits.h>
#include <malloc.h>
#include <new.h>
#include <windows.h>
#include <algorithm>
#include <limits>
#include <Ultralight/private/util/Debug.h>
#include <Ultralight/private/util/MemoryTag.h>
#include <Ultralight/private/tracy/Tracy.hpp>

// Returns true iff |value| is a power of 2.
template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr bool IsPowerOfTwo(T value) {
  // From "Hacker's Delight": Section 2.1 Manipulating Rightmost Bits.
  //
  // Only positive integers with a single bit set are powers of two. If only one
  // bit is set in x (e.g. 0b00000100000000) then |x-1| will have that bit set
  // to zero and all bits to its right set to 1 (e.g. 0b00000011111111). Hence
  // |x & (x-1)| is 0 iff x is a power of two.
  return value > 0 && (value & (value - 1)) == 0;
}

// Round down |size| to a multiple of alignment, which must be a power of two.
inline constexpr size_t AlignDown(size_t size, size_t alignment) { return size & ~(alignment - 1); }

// Move |ptr| back to the previous multiple of alignment, which must be a power
// of two. Defined for types where sizeof(T) is one byte.
template <typename T, typename = typename std::enable_if<sizeof(T) == 1>::type>
inline T* AlignDown(T* ptr, size_t alignment) {
  return reinterpret_cast<T*>(AlignDown(reinterpret_cast<size_t>(ptr), alignment));
}

// Round up |size| to a multiple of alignment, which must be a power of two.
inline constexpr size_t AlignUp(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

// Advance |ptr| to the next multiple of alignment, which must be a power of
// two. Defined for types where sizeof(T) is one byte.
template <typename T, typename = typename std::enable_if<sizeof(T) == 1>::type>
inline T* AlignUp(T* ptr, size_t alignment) {
  return reinterpret_cast<T*>(AlignUp(reinterpret_cast<size_t>(ptr), alignment));
}

bool g_is_win_shim_layer_initialized = false;

namespace {

const size_t kWindowsPageSize = 4096;
const size_t kMaxWindowsAllocation = INT_MAX - kWindowsPageSize;

inline HANDLE get_heap_handle() { return reinterpret_cast<HANDLE>(_get_heap_handle()); }

} // namespace

#if defined(ULTRALIGHT_MODULE_ULTRALIGHTCORE)
static thread_local MemoryTag currentMemoryTag = MemoryTag::Renderer;
#elif defined(ULTRALIGHT_MODULE_WEBCORE) || defined(ULTRALIGHT_MODULE_ULTRALIGHT)
static thread_local MemoryTag currentMemoryTag = MemoryTag::WebCore;
#elif defined(ULTRALIGHT_MODULE_APPCORE)
static thread_local MemoryTag currentMemoryTag = MemoryTag::App;
#else
static thread_local MemoryTag currentMemoryTag = MemoryTag::Uncategorized;
#endif

ScopedMemoryZone::ScopedMemoryZone(MemoryTag tag) : previousTag(currentMemoryTag) {
  currentMemoryTag = tag;
}

ScopedMemoryZone::~ScopedMemoryZone() { currentMemoryTag = previousTag; }

// We always want the prefix to be some multiple of MEMORY_ALLOCATION_ALIGNMENT
union TagStorage {
  MemoryTag val;
  uint8_t pad[MEMORY_ALLOCATION_ALIGNMENT];
};

constexpr size_t TagSize() { return MEMORY_ALLOCATION_ALIGNMENT; }
constexpr size_t TagStorageSize() { return sizeof(TagStorage); }

static_assert(TagSize() == TagStorageSize(), "Tag Size doesn't match Tag Storage Size");

inline void* ProfiledHeapAlloc(size_t size) {
  void* paddedAlloc = HeapAlloc(get_heap_handle(), 0, size + TagSize());
  void* alloc = paddedAlloc ? static_cast<uint8_t*>(paddedAlloc) + TagSize() : paddedAlloc;

  if (alloc) {
    static_cast<TagStorage*>(paddedAlloc)->val = currentMemoryTag;
    ProfileAlloc(alloc, size, MemoryTagToString(currentMemoryTag));
    UpdateMemoryStats(currentMemoryTag, (int64_t)size);
  }

  return alloc;
}

inline void ProfiledHeapFree(void* alloc) {
  void* paddedAlloc = static_cast<uint8_t*>(alloc) - TagSize();
  MemoryTag tag = static_cast<TagStorage*>(paddedAlloc)->val;
  size_t paddedSize = HeapSize(get_heap_handle(), 0, paddedAlloc);
  ProfileFree(alloc, MemoryTagToString(tag));
  UpdateMemoryStats(tag, (int64_t)(paddedSize - TagSize()) * -1);
  HeapFree(get_heap_handle(), 0, paddedAlloc);
}

inline void* ProfiledHeapReAlloc(void* alloc, size_t size) {
  void* paddedAlloc = static_cast<uint8_t*>(alloc) - TagSize();
  MemoryTag oldTag = static_cast<TagStorage*>(paddedAlloc)->val;
  size_t oldPaddedSize = HeapSize(get_heap_handle(), 0, paddedAlloc);
  void* newPaddedAlloc = HeapReAlloc(get_heap_handle(), 0, paddedAlloc, size + TagSize());
  void* newAlloc
      = newPaddedAlloc ? static_cast<uint8_t*>(newPaddedAlloc) + TagSize() : newPaddedAlloc;

  ProfileFree(alloc, MemoryTagToString(oldTag));
  UpdateMemoryStats(oldTag, (int64_t)(oldPaddedSize - TagSize()) * -1);

  if (newPaddedAlloc) {
    ProfileAlloc(newAlloc, size, MemoryTagToString(currentMemoryTag));
    UpdateMemoryStats(currentMemoryTag, (int64_t)size);

    static_cast<TagStorage*>(newPaddedAlloc)->val = currentMemoryTag;
  }

  return newAlloc;
}

inline void* ProfiledHeapReAllocInPlace(void* alloc, size_t size) {
  void* paddedAlloc = static_cast<uint8_t*>(alloc) - TagSize();
  MemoryTag oldTag = static_cast<TagStorage*>(paddedAlloc)->val;
  size_t oldPaddedSize = HeapSize(get_heap_handle(), 0, paddedAlloc);
  void* newPaddedAlloc
      = HeapReAlloc(get_heap_handle(), HEAP_REALLOC_IN_PLACE_ONLY, paddedAlloc, size + TagSize());
  void* newAlloc
      = newPaddedAlloc ? static_cast<uint8_t*>(newPaddedAlloc) + TagSize() : newPaddedAlloc;

  if (newPaddedAlloc) {
    ProfileFree(alloc, MemoryTagToString(oldTag));
    UpdateMemoryStats(oldTag, (int64_t)(oldPaddedSize - TagSize()) * -1);

    ProfileAlloc(newAlloc, size, MemoryTagToString(currentMemoryTag));
    UpdateMemoryStats(currentMemoryTag, (int64_t)size);

    static_cast<TagStorage*>(newPaddedAlloc)->val = currentMemoryTag;
  }

  return newAlloc;
}

void* WinHeapMalloc(size_t size) {
  if (!size)
    return nullptr;

  if (size < kMaxWindowsAllocation) {
    return ProfiledHeapAlloc(size);
  }

  return nullptr;
}

void WinHeapFree(void* ptr) {
  if (!ptr)
    return;

  ProfiledHeapFree(ptr);
}

void* WinHeapRealloc(void* ptr, size_t size) {
  if (!ptr)
    return WinHeapMalloc(size);

  if (!size) {
    WinHeapFree(ptr);
    return nullptr;
  }

  if (size < kMaxWindowsAllocation) {
    return ProfiledHeapReAlloc(ptr, size);
  }

  return nullptr;
}

size_t WinHeapGetSizeEstimate(void* ptr) {
  if (!ptr)
    return 0;

  void* paddedAlloc = static_cast<uint8_t*>(ptr) - TagSize();
  return HeapSize(get_heap_handle(), 0, paddedAlloc);
}

// Call the new handler, if one has been set.
// Returns true on successfully calling the handler, false otherwise.
bool WinCallNewHandler(size_t size) {
#ifdef _CPPUNWIND
#error "Exceptions in allocator shim are not supported!"
#endif // _CPPUNWIND
  // Get the current new handler.
  _PNH nh = _query_new_handler();
  if (!nh)
    return false;
  // Since exceptions are disabled, we don't really know if new_handler
  // failed.  Assume it will abort if it fails.
  return nh(size) ? true : false;
}

// The Windows _aligned_* functions are implemented by creating an allocation
// with enough space to create an aligned allocation internally. The offset to
// the original allocation is prefixed to the aligned allocation so that it can
// be correctly freed.

namespace {

struct AlignedPrefix {
  // Offset to the original allocation point.
  unsigned int original_allocation_offset;
  // Make sure an unsigned int is enough to store the offset
  static_assert(kMaxWindowsAllocation < std::numeric_limits<unsigned int>::max(),
                "original_allocation_offset must be able to fit into an unsigned int");
};

// Compute how large an allocation we need to fit an allocation with the given
// size and alignment and space for a prefix pointer.
size_t AdjustedSize(size_t size, size_t alignment) {
  // Minimal alignment is the prefix size so the prefix is properly aligned.
  alignment = std::max(alignment, alignof(AlignedPrefix));
  return size + sizeof(AlignedPrefix) + alignment - 1;
}

// Align the allocation and write the prefix.
void* AlignAllocation(void* ptr, size_t alignment) {
  // Minimal alignment is the prefix size so the prefix is properly aligned.
  alignment = std::max(alignment, alignof(AlignedPrefix));

  uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
  address = AlignUp(address + sizeof(AlignedPrefix), alignment);

  // Write the prefix.
  AlignedPrefix* prefix = reinterpret_cast<AlignedPrefix*>(address) - 1;
  prefix->original_allocation_offset = address - reinterpret_cast<uintptr_t>(ptr);
  return reinterpret_cast<void*>(address);
}

// Return the original allocation from an aligned allocation.
void* UnalignAllocation(void* ptr) {
  AlignedPrefix* prefix = reinterpret_cast<AlignedPrefix*>(ptr) - 1;
  void* unaligned = static_cast<uint8_t*>(ptr) - prefix->original_allocation_offset;
  return unaligned;
}

} // namespace

void* WinHeapAlignedMalloc(size_t size, size_t alignment) {
  UL_CHECK(IsPowerOfTwo(alignment));

  size_t adjusted = AdjustedSize(size, alignment);
  if (adjusted >= kMaxWindowsAllocation)
    return nullptr;

  void* ptr = WinHeapMalloc(adjusted);
  if (!ptr)
    return nullptr;

  return AlignAllocation(ptr, alignment);
}

void* WinHeapAlignedRealloc(void* ptr, size_t size, size_t alignment) {
  UL_CHECK(IsPowerOfTwo(alignment));

  if (!ptr)
    return WinHeapAlignedMalloc(size, alignment);
  if (!size) {
    WinHeapAlignedFree(ptr);
    return nullptr;
  }

  size_t adjusted = AdjustedSize(size, alignment);
  if (adjusted >= kMaxWindowsAllocation)
    return nullptr;

  // Try to resize the allocation in place first.
  void* unaligned = UnalignAllocation(ptr);
  if (ProfiledHeapReAllocInPlace(unaligned, adjusted)) {
    return ptr;
  }

  // Otherwise manually perform an _aligned_malloc() and copy since an
  // unaligned allocation from HeapReAlloc() would force us to copy the
  // allocation twice.
  void* new_ptr = WinHeapAlignedMalloc(size, alignment);
  if (!new_ptr)
    return nullptr;

  size_t gap = reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(unaligned);
  size_t old_size = WinHeapGetSizeEstimate(unaligned) - gap;
  memcpy(new_ptr, ptr, std::min(size, old_size));
  WinHeapAlignedFree(ptr);
  return new_ptr;
}

void WinHeapAlignedFree(void* ptr) {
  if (!ptr)
    return;

  void* original_allocation = UnalignAllocation(ptr);
  WinHeapFree(original_allocation);
}

#endif