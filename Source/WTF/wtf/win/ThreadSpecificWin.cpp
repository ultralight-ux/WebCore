/*
 * Copyright (C) 2009 Jian Li <jianli@chromium.org>
 * Copyright (C) 2012 Patrick Gansterer <paroga@paroga.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include <wtf/ThreadSpecific.h>
#include <wtf/WordLock.h>
#include <set>

#if !USE(PTHREADS)

namespace WTF {

    /*
long& flsKeyCount()
{
    static long count;
    return count;
}

DWORD* flsKeys()
{
    static DWORD keys[maxFlsKeySize];
    return keys;
}
*/

std::set<DWORD> g_flsKeyStorage;
WordLock g_flsKeyStorageLock;

ThreadSpecificKey flsKeyCreate(void(THREAD_SPECIFIC_CALL* destructor)(void*))
{
    ThreadSpecificKey result = FlsAlloc(destructor);
    if (result != FLS_OUT_OF_INDEXES) {
        g_flsKeyStorageLock.lock();
        g_flsKeyStorage.insert(result);
        g_flsKeyStorageLock.unlock();
    }

    return result;
}

void flsKeyDestroy(ThreadSpecificKey key)
{
    if (key != FLS_OUT_OF_INDEXES) {
        g_flsKeyStorageLock.lock();
        g_flsKeyStorage.erase(key);
        g_flsKeyStorageLock.unlock();
        FlsFree(key);
    }
}

void flsKeyDestroyAll()
{
    g_flsKeyStorageLock.lock();
    for (auto key : g_flsKeyStorage) {
        FlsSetValue(key, 0);
        FlsFree(key);
    }
    g_flsKeyStorageLock.unlock();
}

} // namespace WTF

#endif // !USE(PTHREADS)
