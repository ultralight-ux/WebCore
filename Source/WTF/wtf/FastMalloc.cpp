/*
 * Copyright (c) 2005, 2007, Google Inc. All rights reserved.
 * Copyright (C) 2005-2018 Apple Inc. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include <wtf/FastMalloc.h>

#include <limits>
#include <string.h>
#include <wtf/CheckedArithmetic.h>
#include <wtf/DataLog.h>

#if OS(WINDOWS)
#include <windows.h>
#else
#include <pthread.h>
#if HAVE(RESOURCE_H)
#include <sys/resource.h>
#endif // HAVE(RESOURCE_H)
#endif

#if OS(DARWIN)
#include <mach/mach_init.h>
#include <malloc/malloc.h>
#endif

#if USE(ULTRALIGHT)
#include <Ultralight/private/tracy/Tracy.hpp>
#endif

namespace WTF {

#if !defined(NDEBUG)
namespace {
// We do not use std::numeric_limits<size_t>::max() here due to the edge case in VC++.
// https://bugs.webkit.org/show_bug.cgi?id=173720
static size_t maxSingleAllocationSize = SIZE_MAX;
};

void fastSetMaxSingleAllocationSize(size_t size)
{
    maxSingleAllocationSize = size;
}

#define ASSERT_IS_WITHIN_LIMIT(size) do { \
        size_t size__ = (size); \
        ASSERT_WITH_MESSAGE((size__) <= maxSingleAllocationSize, "Requested size (%zu) exceeds max single allocation size set for testing (%zu)", (size__), maxSingleAllocationSize); \
    } while (false)

#define FAIL_IF_EXCEEDS_LIMIT(size) do { \
        if (UNLIKELY((size) > maxSingleAllocationSize)) \
            return nullptr; \
    } while (false)

#else // !defined(NDEBUG)

#define ASSERT_IS_WITHIN_LIMIT(size)
#define FAIL_IF_EXCEEDS_LIMIT(size)

#endif // !defined(NDEBUG)


} // namespace WTF

#if USE(SYSTEM_MALLOC) && !USE(MIMALLOC)

#include <wtf/OSAllocator.h>

#if OS(WINDOWS)
#include <malloc.h>
#endif

#if defined(TRACY_PROFILE_MEMORY)
void operator delete(void* p) noexcept
{
    ProfileFree(p);
    free(p);
};

void operator delete[](void* p) noexcept
{
    ProfileFree(p);
    free(p);
};

void* operator new(std::size_t n) noexcept(false)
{
    auto p = malloc(n);
    ProfileAlloc(p, n);
    return p;
}

void* operator new[](std::size_t n) noexcept(false)
{
    auto p = malloc(n);
    ProfileAlloc(p, n);
    return p;
}

void* operator new(std::size_t n, const std::nothrow_t& tag) noexcept
{
    (void)(tag);
    auto p = malloc(n);
    ProfileAlloc(p, n);
    return p;
}

void* operator new[](std::size_t n, const std::nothrow_t& tag) noexcept
{
    (void)(tag);
    auto p = malloc(n);
    ProfileAlloc(p, n);
    return p;
}

#endif

namespace WTF {

bool isFastMallocEnabled()
{
    return false;
}

size_t fastMallocGoodSize(size_t bytes)
{
#if OS(DARWIN)
    return malloc_good_size(bytes);
#else
    return bytes;
#endif
}

void* fastZeroedMalloc(size_t n)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    void* result = fastMalloc(n);
    memset(result, 0, n);
    return result;
}

char* fastStrDup(const char* src)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    size_t len = strlen(src) + 1;
    char* dup = static_cast<char*>(fastMalloc(len));
    memcpy(dup, src, len);
    return dup;
}

TryMallocReturnValue tryFastZeroedMalloc(size_t n)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    void* result;
    if (!tryFastMalloc(n).getValue(result))
        return 0;
    memset(result, 0, n);
    return result;
}

#if OS(WINDOWS)

void* fastAlignedMalloc(size_t alignment, size_t size) 
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    ASSERT_IS_WITHIN_LIMIT(size);
    void* p = _aligned_malloc(size, alignment);
    if (UNLIKELY(!p))
        CRASH();
    ProfileAlloc(p, size);
    return p;
}

void* tryFastAlignedMalloc(size_t alignment, size_t size) 
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    FAIL_IF_EXCEEDS_LIMIT(size);
    auto p = _aligned_malloc(size, alignment);
    ProfileAlloc(p, size);
    return p;
}

void fastAlignedFree(void* p) 
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    ProfileFree(p);
    _aligned_free(p);
}

#else

void* fastAlignedMalloc(size_t alignment, size_t size) 
{
    ASSERT_IS_WITHIN_LIMIT(size);
    void* p = nullptr;
    posix_memalign(&p, alignment, size);
    if (UNLIKELY(!p))
        CRASH();
    ProfileAlloc(p, size);
    return p;
}

void* tryFastAlignedMalloc(size_t alignment, size_t size) 
{
    FAIL_IF_EXCEEDS_LIMIT(size);
    void* p = nullptr;
    posix_memalign(&p, alignment, size);
    ProfileAlloc(p, size);
    return p;
}

void fastAlignedFree(void* p) 
{
    ProfileFree(p);
    free(p);
}

#endif // OS(WINDOWS)

TryMallocReturnValue tryFastMalloc(size_t n) 
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    FAIL_IF_EXCEEDS_LIMIT(n);
    auto p = malloc(n);
    ProfileAlloc(p, n);
    return p;
}

void* fastMalloc(size_t n) 
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    ASSERT_IS_WITHIN_LIMIT(n);
    void* result = malloc(n);
    if (!result)
        CRASH();
    ProfileAlloc(result, n);
    return result;
}

TryMallocReturnValue tryFastCalloc(size_t n_elements, size_t element_size)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    FAIL_IF_EXCEEDS_LIMIT(n_elements * element_size);
    auto p = calloc(n_elements, element_size);
    ProfileAlloc(p, n_elements * element_size);
    return p;
}

void* fastCalloc(size_t n_elements, size_t element_size)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    ASSERT_IS_WITHIN_LIMIT(n_elements * element_size);
    void* result = calloc(n_elements, element_size);
    if (!result)
        CRASH();
    ProfileAlloc(result, n_elements * element_size);
    return result;
}

void fastFree(void* p)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    ProfileFree(p);
    free(p);
}

void* traceRealloc(void* p, size_t n)
{
    void* result;

    if (!p) {
        result = malloc(n);
        ProfileAlloc(result, n);
        return result;
    } else {
        result = malloc(n);
        ProfileAlloc(result, n);

        if (!result) {
            return nullptr;
        }

        memcpy(result, p, std::min(n, fastMallocSize(p)));

        ProfileFree(p);
        free(p);
    }

    return result;
}

void* fastRealloc(void* p, size_t n)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    ASSERT_IS_WITHIN_LIMIT(n);
#if defined(TRACY_PROFILE_MEMORY)
    void* result = traceRealloc(p, n);
#else
    void* result = realloc(p, n);
#endif
    if (!result)
        CRASH();
    return result;
}

TryMallocReturnValue tryFastRealloc(void* p, size_t n)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    FAIL_IF_EXCEEDS_LIMIT(n);
#if defined(TRACY_PROFILE_MEMORY)
    void* result = traceRealloc(p, n);
#else
    void* result = realloc(p, n);
#endif
    return result;
}

void releaseFastMallocFreeMemory() { }
void releaseFastMallocFreeMemoryForThisThread() { }
    
FastMallocStatistics fastMallocStatistics()
{
    FastMallocStatistics statistics = { 0, 0, 0 };
    return statistics;
}

size_t fastMallocSize(const void* p)
{
#if OS(DARWIN)
    return malloc_size(p);
#elif OS(WINDOWS)
    return _msize(const_cast<void*>(p));
#else
    UNUSED_PARAM(p);
    return 1;
#endif
}

void fastCommitAlignedMemory(void* ptr, size_t size)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    OSAllocator::commit(ptr, size, true, false);
}

void fastDecommitAlignedMemory(void* ptr, size_t size)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    OSAllocator::decommit(ptr, size);
}

void fastEnableMiniMode() { }

} // namespace WTF

#elif USE(SYSTEM_MALLOC) && USE(MIMALLOC)

#include <mimalloc.h>
#include <wtf/OSAllocator.h>

namespace WTF {

bool isFastMallocEnabled()
{
    return false;
}

WTF_PRIVATE_INLINE void* fastZeroedMalloc(size_t n)
{
    ASSERT_IS_WITHIN_LIMIT(n);
    return mi_zalloc(n);
}

WTF_PRIVATE_INLINE char* fastStrDup(const char* src)
{
    size_t len = strlen(src) + 1;
    char* dup = static_cast<char*>(fastMalloc(len));
    memcpy(dup, src, len);
    return dup;
}

WTF_PRIVATE_INLINE TryMallocReturnValue tryFastZeroedMalloc(size_t n)
{
    return fastZeroedMalloc(n);
}

WTF_PRIVATE_INLINE void* fastMalloc(size_t size)
{
    ASSERT_IS_WITHIN_LIMIT(size);
    return mi_malloc(size);
}

WTF_PRIVATE_INLINE void* fastCalloc(size_t numElements, size_t elementSize)
{
    ASSERT_IS_WITHIN_LIMIT(numElements * elementSize);
    return mi_calloc(numElements, elementSize);
}

WTF_PRIVATE_INLINE void* fastRealloc(void* object, size_t size)
{
    ASSERT_IS_WITHIN_LIMIT(size);
    return mi_realloc(object, size);
}

WTF_PRIVATE_INLINE void fastFree(void* object)
{
    mi_free(object);
}

WTF_PRIVATE_INLINE size_t fastMallocSize(const void*)
{
    // FIXME: This is incorrect; best fix is probably to remove this function.
    // Caller currently are all using this for assertion, not to actually check
    // the size of the allocation, so maybe we can come up with something for that.
    return 1;
}

WTF_PRIVATE_INLINE size_t fastMallocGoodSize(size_t size)
{
    return size;
}

WTF_PRIVATE_INLINE void* fastAlignedMalloc(size_t alignment, size_t size) 
{
    ASSERT_IS_WITHIN_LIMIT(size);
    return mi_aligned_alloc(alignment, size);
}

WTF_PRIVATE_INLINE void* tryFastAlignedMalloc(size_t alignment, size_t size) 
{
    FAIL_IF_EXCEEDS_LIMIT(size);
    return mi_aligned_alloc(alignment, size);
}

WTF_PRIVATE_INLINE void fastAlignedFree(void* p) 
{
    mi_free(p);
}

WTF_PRIVATE_INLINE TryMallocReturnValue tryFastMalloc(size_t size)
{
    FAIL_IF_EXCEEDS_LIMIT(size);
    return mi_malloc(size);
}
    
WTF_PRIVATE_INLINE TryMallocReturnValue tryFastCalloc(size_t numElements, size_t elementSize)
{
    FAIL_IF_EXCEEDS_LIMIT(numElements * elementSize);
    Checked<size_t, RecordOverflow> checkedSize = elementSize;
    checkedSize *= numElements;
    if (checkedSize.hasOverflowed())
        return nullptr;
    return mi_calloc(numElements, elementSize);
}
    
WTF_PRIVATE_INLINE TryMallocReturnValue tryFastRealloc(void* object, size_t newSize)
{
    FAIL_IF_EXCEEDS_LIMIT(newSize);
    return mi_realloc(object, newSize);
}

void releaseFastMallocFreeMemory() { 
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    mi_collect(true);
}

void releaseFastMallocFreeMemoryForThisThread() {
    releaseFastMallocFreeMemory();
}
    
FastMallocStatistics fastMallocStatistics()
{
    FastMallocStatistics statistics = { 0, 0, 0 };
    return statistics;
}

void fastCommitAlignedMemory(void* ptr, size_t size)
{
    OSAllocator::commit(ptr, size, true, false);
}

void fastDecommitAlignedMemory(void* ptr, size_t size)
{
    OSAllocator::decommit(ptr, size);
}

void fastEnableMiniMode()
{
}

} // namespace WTF
#else // defined(USE_SYSTEM_MALLOC) && USE_SYSTEM_MALLOC

#include <bmalloc/bmalloc.h>

namespace WTF {

bool isFastMallocEnabled()
{
    return bmalloc::api::isEnabled();
}

void* fastZeroedMalloc(size_t n)
{
    void* result = fastMalloc(n);
    memset(result, 0, n);
    return result;
}

char* fastStrDup(const char* src)
{
    size_t len = strlen(src) + 1;
    char* dup = static_cast<char*>(fastMalloc(len));
    memcpy(dup, src, len);
    return dup;
}

TryMallocReturnValue tryFastZeroedMalloc(size_t n)
{
    void* result;
    if (!tryFastMalloc(n).getValue(result))
        return 0;
    memset(result, 0, n);
    return result;
}

void* fastMalloc(size_t size)
{
    ASSERT_IS_WITHIN_LIMIT(size);
    return bmalloc::api::malloc(size);
}

void* fastCalloc(size_t numElements, size_t elementSize)
{
    ASSERT_IS_WITHIN_LIMIT(numElements * elementSize);
    Checked<size_t> checkedSize = elementSize;
    checkedSize *= numElements;
    void* result = fastZeroedMalloc(checkedSize.unsafeGet());
    if (!result)
        CRASH();
    return result;
}

void* fastRealloc(void* object, size_t size)
{
    ASSERT_IS_WITHIN_LIMIT(size);
    return bmalloc::api::realloc(object, size);
}

void fastFree(void* object)
{
    bmalloc::api::free(object);
}

size_t fastMallocSize(const void*)
{
    // FIXME: This is incorrect; best fix is probably to remove this function.
    // Caller currently are all using this for assertion, not to actually check
    // the size of the allocation, so maybe we can come up with something for that.
    return 1;
}

size_t fastMallocGoodSize(size_t size)
{
    return size;
}

void* fastAlignedMalloc(size_t alignment, size_t size) 
{
    ASSERT_IS_WITHIN_LIMIT(size);
    return bmalloc::api::memalign(alignment, size);
}

void* tryFastAlignedMalloc(size_t alignment, size_t size) 
{
    FAIL_IF_EXCEEDS_LIMIT(size);
    return bmalloc::api::tryMemalign(alignment, size);
}

void fastAlignedFree(void* p) 
{
    bmalloc::api::free(p);
}

TryMallocReturnValue tryFastMalloc(size_t size)
{
    FAIL_IF_EXCEEDS_LIMIT(size);
    return bmalloc::api::tryMalloc(size);
}
    
TryMallocReturnValue tryFastCalloc(size_t numElements, size_t elementSize)
{
    FAIL_IF_EXCEEDS_LIMIT(numElements * elementSize);
    Checked<size_t, RecordOverflow> checkedSize = elementSize;
    checkedSize *= numElements;
    if (checkedSize.hasOverflowed())
        return nullptr;
    return tryFastZeroedMalloc(checkedSize.unsafeGet());
}
    
TryMallocReturnValue tryFastRealloc(void* object, size_t newSize)
{
    FAIL_IF_EXCEEDS_LIMIT(newSize);
    return bmalloc::api::tryRealloc(object, newSize);
}

void releaseFastMallocFreeMemoryForThisThread()
{
    bmalloc::api::scavengeThisThread();
}

void releaseFastMallocFreeMemory()
{
    bmalloc::api::scavenge();
}

FastMallocStatistics fastMallocStatistics()
{

    // FIXME: Can bmalloc itself report the stats instead of relying on the OS?
    FastMallocStatistics statistics;
    statistics.freeListBytes = 0;
    statistics.reservedVMBytes = 0;

#if OS(WINDOWS)
    PROCESS_MEMORY_COUNTERS resourceUsage;
    GetProcessMemoryInfo(GetCurrentProcess(), &resourceUsage, sizeof(resourceUsage));
    statistics.committedVMBytes = resourceUsage.PeakWorkingSetSize;
#elif HAVE(RESOURCE_H)
    struct rusage resourceUsage;
    getrusage(RUSAGE_SELF, &resourceUsage);

#if OS(DARWIN)
    statistics.committedVMBytes = resourceUsage.ru_maxrss;
#else
    statistics.committedVMBytes = resourceUsage.ru_maxrss * 1024;
#endif // OS(DARWIN)

#endif // OS(WINDOWS)
    return statistics;
}

void fastCommitAlignedMemory(void* ptr, size_t size)
{
    bmalloc::api::commitAlignedPhysical(ptr, size);
}

void fastDecommitAlignedMemory(void* ptr, size_t size)
{
    bmalloc::api::decommitAlignedPhysical(ptr, size);
}

void fastEnableMiniMode()
{
    bmalloc::api::enableMiniMode();
}

} // namespace WTF

#endif // defined(USE_SYSTEM_MALLOC) && USE_SYSTEM_MALLOC
