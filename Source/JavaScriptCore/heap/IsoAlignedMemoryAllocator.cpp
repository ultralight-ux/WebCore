/*
 * Copyright (C) 2017-2021 Apple Inc. All rights reserved.
 *
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
#if USE(ULTRALIGHT)

#include "IsoAlignedMemoryAllocator.h"
#include "MarkedBlock.h"
#include <wtf/FastMalloc.h>

namespace JSC {

IsoAlignedMemoryAllocator::IsoAlignedMemoryAllocator(CString name)
    : Base(name)
{
}

IsoAlignedMemoryAllocator::~IsoAlignedMemoryAllocator()
{
    releaseMemoryFromSubclassDestructor();
}

void IsoAlignedMemoryAllocator::dump(PrintStream& out) const
{
    out.print("Iso(", RawPointer(this), ")");
}

void* IsoAlignedMemoryAllocator::tryAllocateMemory(size_t size)
{
#if ENABLE(MALLOC_HEAP_BREAKDOWN)
    return m_debugHeap.malloc(size);
#else
    return FastMalloc::tryMalloc(size);
#endif
}

void IsoAlignedMemoryAllocator::freeMemory(void* pointer)
{
#if ENABLE(MALLOC_HEAP_BREAKDOWN)
    m_debugHeap.free(pointer);
#else
    FastMalloc::free(pointer);
#endif
}

void* IsoAlignedMemoryAllocator::tryReallocateMemory(void*, size_t)
{
    // In IsoSubspace-managed PreciseAllocation, we must not perform realloc.
    RELEASE_ASSERT_NOT_REACHED();
}

void* IsoAlignedMemoryAllocator::tryMallocBlock()
{
    return tryFastAlignedMalloc(MarkedBlock::blockSize, MarkedBlock::blockSize);
}

void IsoAlignedMemoryAllocator::freeBlock(void* block)
{
    fastAlignedFree(block);
}

void IsoAlignedMemoryAllocator::commitBlock(void* block)
{
    WTF::fastCommitAlignedMemory(block, MarkedBlock::blockSize);
}

void IsoAlignedMemoryAllocator::decommitBlock(void* block)
{
    WTF::fastDecommitAlignedMemory(block, MarkedBlock::blockSize);
}

} // namespace JSC

#elif OS(WINDOWS)

#include "IsoAlignedMemoryAllocator.h"
#include "MarkedBlock.h"
#include <wtf/OSAllocator.h>
#include <Ultralight/private/util/MemoryTag.h>
#include <Ultralight/private/tracy/Tracy.hpp>

namespace JSC {

IsoAlignedMemoryAllocator::IsoAlignedMemoryAllocator(CString name)
    : Base(name)
{
}

IsoAlignedMemoryAllocator::~IsoAlignedMemoryAllocator()
{
    for (unsigned i = 0; i < m_blocks.size(); ++i) {
        void* block = m_blocks[i];
        if (m_committed[i]) {
            OSAllocator::decommitAndRelease(block, MarkedBlock::blockSize);
            ProfileFree(block, MemoryTagToString(MemoryTag::JavaScript));
        } else {
            OSAllocator::releaseDecommitted(block, MarkedBlock::blockSize);
        }
    }
}

void* IsoAlignedMemoryAllocator::tryAllocateAlignedMemory(size_t alignment, size_t size)
{
    // Since this is designed specially for IsoSubspace, we know that we will only be asked to
    // allocate MarkedBlocks.
    RELEASE_ASSERT(alignment == MarkedBlock::blockSize);
    RELEASE_ASSERT(size == MarkedBlock::blockSize);
    
    auto locker = holdLock(m_lock);
    
    m_firstUncommitted = m_committed.findBit(m_firstUncommitted, false);
    if (m_firstUncommitted < m_blocks.size()) {
        m_committed[m_firstUncommitted] = true;
        void* result = m_blocks[m_firstUncommitted];
        OSAllocator::commit(result, MarkedBlock::blockSize, true, false);
        ProfileAlloc(result, MarkedBlock::blockSize, MemoryTagToString(MemoryTag::JavaScript));
        return result;
    }
    
    void* result = OSAllocator::reserveAndCommit(MarkedBlock::blockSize, MarkedBlock::blockSize,
        WTF::OSAllocator::Usage::FastMallocPages, true, false);
    ProfileAlloc(result, MarkedBlock::blockSize, MemoryTagToString(MemoryTag::JavaScript));

    if (!result)
        return nullptr;
    unsigned index = m_blocks.size();
    m_blocks.append(result);
    m_blockIndices.add(result, index);
    if (m_blocks.capacity() != m_committed.size())
        m_committed.resize(m_blocks.capacity());
    m_committed[index] = true;
    return result;
}

void IsoAlignedMemoryAllocator::freeAlignedMemory(void* basePtr)
{
    auto locker = holdLock(m_lock);
    
    auto iter = m_blockIndices.find(basePtr);
    RELEASE_ASSERT(iter != m_blockIndices.end());
    unsigned index = iter->value;
    m_committed[index] = false;
    m_firstUncommitted = std::min(index, m_firstUncommitted);
    OSAllocator::decommit(basePtr, MarkedBlock::blockSize);
    ProfileFree(basePtr, MemoryTagToString(MemoryTag::JavaScript));
}

void IsoAlignedMemoryAllocator::dump(PrintStream& out) const
{
    out.print("Iso(", RawPointer(this), ")");
}

void* IsoAlignedMemoryAllocator::tryAllocateMemory(size_t)
{
    RELEASE_ASSERT_NOT_REACHED();
}

void IsoAlignedMemoryAllocator::freeMemory(void*)
{
    RELEASE_ASSERT_NOT_REACHED();
}

void* IsoAlignedMemoryAllocator::tryReallocateMemory(void*, size_t)
{
    RELEASE_ASSERT_NOT_REACHED();
}

} // namespace JSC

#else

#include "IsoAlignedMemoryAllocator.h"
#include "MarkedBlock.h"

namespace JSC {

IsoAlignedMemoryAllocator::IsoAlignedMemoryAllocator(CString name)
    : Base(name)
{
}

IsoAlignedMemoryAllocator::~IsoAlignedMemoryAllocator()
{
    releaseMemoryFromSubclassDestructor();
}

void IsoAlignedMemoryAllocator::dump(PrintStream& out) const
{
    out.print("Iso(", RawPointer(this), ")");
}

void* IsoAlignedMemoryAllocator::tryAllocateMemory(size_t size)
{
#if ENABLE(MALLOC_HEAP_BREAKDOWN)
    return m_debugHeap.malloc(size);
#else
    return FastMalloc::tryMalloc(size);
#endif
}

void IsoAlignedMemoryAllocator::freeMemory(void* pointer)
{
#if ENABLE(MALLOC_HEAP_BREAKDOWN)
    m_debugHeap.free(pointer);
#else
    FastMalloc::free(pointer);
#endif
}

void* IsoAlignedMemoryAllocator::tryReallocateMemory(void*, size_t)
{
    // In IsoSubspace-managed PreciseAllocation, we must not perform realloc.
    RELEASE_ASSERT_NOT_REACHED();
}

void* IsoAlignedMemoryAllocator::tryMallocBlock()
{
    return tryFastAlignedMalloc(MarkedBlock::blockSize, MarkedBlock::blockSize);
}

void IsoAlignedMemoryAllocator::freeBlock(void* block)
{
    fastAlignedFree(block);
}

void IsoAlignedMemoryAllocator::commitBlock(void* block)
{
    WTF::fastCommitAlignedMemory(block, MarkedBlock::blockSize);
}

void IsoAlignedMemoryAllocator::decommitBlock(void* block)
{
    WTF::fastDecommitAlignedMemory(block, MarkedBlock::blockSize);
}

} // namespace JSC

#endif // OS(WINDOWS)