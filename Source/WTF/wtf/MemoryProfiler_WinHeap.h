///
/// Copyright (C) 2022 Ultralight, Inc. All rights reserved.
///

#pragma once

#if (defined(TRACY_PROFILE_MEMORY) || defined(ULTRALIGHT_ENABLE_MEMORY_STATS)) && defined(_WIN32)

// Set to true if the link-time magic has successfully hooked into the CRT's
// heap initialization.
extern bool g_is_win_shim_layer_initialized;

// Thin wrappers to implement the standard C allocation semantics on the
// CRT's Windows heap.
void* WinHeapMalloc(size_t size);

void WinHeapFree(void* ptr);

void* WinHeapRealloc(void* ptr, size_t size);

// Returns a lower-bound estimate for the full amount of memory consumed by the
// the allocation |ptr|.
size_t WinHeapGetSizeEstimate(void* ptr);

// Call the new handler, if one has been set.
// Returns true on successfully calling the handler, false otherwise.
bool WinCallNewHandler(size_t size);

// Wrappers to implement the interface for the _aligned_* functions on top of
// the CRT's Windows heap. Exported for tests.
void* WinHeapAlignedMalloc(size_t size, size_t alignment);

void* WinHeapAlignedRealloc(void* ptr, size_t size, size_t alignment);

void WinHeapAlignedFree(void* ptr);

#endif
