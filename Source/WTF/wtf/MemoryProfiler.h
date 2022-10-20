///
/// Copyright (C) 2022 Ultralight, Inc. All rights reserved.
///
#ifndef __ULTRALIGHT_MEMORY_PROFILER_H__
#define __ULTRALIGHT_MEMORY_PROFILER_H__

#include <Ultralight/private/util/MemoryTag.h>

#if defined(ULTRALIGHT_ENABLE_MEMORY_PROFILER)

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

#endif // __ULTRALIGHT_MEMORY_PROFILER_H__