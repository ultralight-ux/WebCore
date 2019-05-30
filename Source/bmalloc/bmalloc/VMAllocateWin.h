/*
 * Copyright (C) 2016 Naver Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VMAllocateWin_h
#define VMAllocateWin_h

#if BOS(WINDOWS)

#include <windows.h>

namespace bmalloc {

inline void* vmCommit(void* ptr, size_t size)
{
    size_t commitedBytes = 0;
    while (commitedBytes < size) {
        MEMORY_BASIC_INFORMATION info;
        char* basePtr = static_cast<char*>(ptr) + commitedBytes;
        VirtualQuery(basePtr, &info, sizeof(MEMORY_BASIC_INFORMATION));
        size_t commitBytes = std::min(static_cast<size_t>(info.RegionSize), size - commitedBytes);
        if (info.State != MEM_COMMIT) {
            BASSERT(info.State == MEM_RESERVE);
            RELEASE_BASSERT(VirtualAlloc(basePtr, commitBytes, MEM_COMMIT, PAGE_READWRITE));
        }
        commitedBytes += commitBytes;
    }

    return ptr;
}

inline bool vmDecommit(void* ptr, size_t size)
{
    size_t decommitedBytes = 0;
    while (decommitedBytes < size) {
        MEMORY_BASIC_INFORMATION info;
        char* basePtr = static_cast<char*>(ptr) + decommitedBytes;
        VirtualQuery(basePtr, &info, sizeof(MEMORY_BASIC_INFORMATION));
        size_t decommitBytes = std::min(static_cast<size_t>(info.RegionSize), size - decommitedBytes);
        VirtualFree(basePtr, decommitBytes, MEM_DECOMMIT);
        decommitedBytes += decommitBytes;
    }

    return true;
}

inline bool vmDecommitAndRelease(void* ptr, size_t size)
{
    size_t decommitedBytes = 0;
    while (decommitedBytes < size) {
        MEMORY_BASIC_INFORMATION info;
        char* basePtr = static_cast<char*>(ptr) + decommitedBytes;
        VirtualQuery(basePtr, &info, sizeof(MEMORY_BASIC_INFORMATION));
        size_t decommitBytes = std::min(static_cast<size_t>(info.RegionSize), size - decommitedBytes);
        VirtualFree(basePtr, decommitBytes, MEM_DECOMMIT);
        decommitedBytes += decommitBytes;

        VirtualQuery(info.AllocationBase, &info, sizeof(MEMORY_BASIC_INFORMATION));
        if (info.State == MEM_RESERVE) {
            char* allocationBase = static_cast<char*>(info.AllocationBase);
            VirtualQuery(allocationBase + info.RegionSize, &info, sizeof(MEMORY_BASIC_INFORMATION));
            if (info.AllocationBase != allocationBase)
                RELEASE_BASSERT(VirtualFree(allocationBase, 0, MEM_RELEASE));
        }
    }

    return true;
}

void vmValidate(size_t vmSize);

inline void* tryVMReserve(size_t vmSize)
{
    vmValidate(vmSize);
    void* result = VirtualAlloc(nullptr, vmSize, MEM_RESERVE, PAGE_READWRITE);
    RELEASE_BASSERT(result);
    return result;
}

} // namespace bmalloc

#endif // BOS(WINDOWS)

#endif // VMAllocateWin_h
