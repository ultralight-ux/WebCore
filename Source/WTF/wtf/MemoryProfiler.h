///
/// Copyright (C) 2022 Ultralight, Inc. All rights reserved.
///

#pragma once

#include <Ultralight/private/util/MemoryTag.h>

#if (defined(TRACY_PROFILE_MEMORY) || defined(ULTRALIGHT_ENABLE_MEMORY_STATS)) && defined(_WIN32)

class ScopedMemoryZone {
 public:
  ScopedMemoryZone(MemoryTag tag);
  ~ScopedMemoryZone();

 protected:
  MemoryTag previousTag;
};

// All allocations made inside this scope will be tagged with the specified MemoryTag
#define ProfiledMemoryZone(tag) ScopedMemoryZone ____scoped_memory_zone(tag);

#else
#define ProfiledMemoryZone(tag)
#endif
