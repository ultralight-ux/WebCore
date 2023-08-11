/*
 * Copyright (C) 2010 Google Inc.  All rights reserved.
 * Copyright (C) 2012, 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "AsyncFileStream.h"

#include "FileStream.h"
#include "FileStreamClient.h"
#include <mutex>
#include <wtf/AutodrainedPool.h>
#include <wtf/Function.h>
#include <wtf/MainThread.h>
#include <wtf/MessageQueue.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/Threading.h>
#include <wtf/URL.h>
#include <wtf/threads/BinarySemaphore.h>
#include <wtf/Shutdown.h>

namespace WebCore {

struct AsyncFileStream::Internals {
    WTF_MAKE_STRUCT_FAST_ALLOCATED;

    explicit Internals(FileStreamClient&);

    FileStream stream;
    FileStreamClient& client;
#if !COMPILER(MSVC)
    std::atomic_bool destroyed { false };
#else
    std::atomic_bool destroyed;
#endif
};

inline AsyncFileStream::Internals::Internals(FileStreamClient& client)
    : client(client)
{
#if COMPILER(MSVC)
    // Work around a bug that prevents the default value above from compiling.
    atomic_init(&destroyed, false);
#endif
}

class FileThread {
public:
    FileThread()
        : m_stopping(false)
    {
        m_thread = Thread::create("WebCore: AsyncFileStream", [this] {
            for (;;) {
                AutodrainedPool pool;
                auto function = m_queue.waitForMessage();
                (*function)();
                if (m_stopping)
                    break;
            }
            m_stopSemaphore.signal();
        });
    }

    ~FileThread()
    {
        if (m_thread)
            stop();
    }

    void stop()
    {
        if (m_thread && !m_stopping) {
            m_stopping = true;
            auto stopFunc = []() {};
            m_queue.append(makeUnique<Function<void()>>(WTFMove(stopFunc)));
            m_stopSemaphore.wait();
            m_thread->waitForCompletion();
        }

        m_thread = nullptr;
    }

    void callOnThread(Function<void()>&& function)
    {
        if (m_stopping)
            return;

        m_queue.append(makeUnique<Function<void()>>(WTFMove(function)));
    }

protected:
    std::atomic<bool> m_stopping;
    BinarySemaphore m_stopSemaphore;
    RefPtr<Thread> m_thread;
    MessageQueue<Function<void()>> m_queue;
};

static FileThread* g_thread = nullptr;

static void callOnFileThread(Function<void()>&& function)
{
    ASSERT(isMainThread());
    ASSERT(function);

    if (!g_thread) {
        g_thread = new FileThread();

        WTF::CallOnShutdown([]() mutable {
            if (g_thread) {
                delete g_thread;
                g_thread = nullptr;
            }
        },
            WTF::ShutdownPriority::High);
    }

    g_thread->callOnThread(WTFMove(function));
}

AsyncFileStream::AsyncFileStream(FileStreamClient& client)
    : m_internals(makeUnique<Internals>(client))
{
    ASSERT(isMainThread());
}

AsyncFileStream::~AsyncFileStream()
{
    ASSERT(isMainThread());

    // Set flag to prevent client callbacks and also prevent queued operations from starting.
    m_internals->destroyed = true;

    // Call through file thread and back to main thread to make sure deletion happens
    // after all file thread functions and all main thread functions called from them.
    callOnFileThread([internals = WTFMove(m_internals)]() mutable {
        callOnMainThread([internals = WTFMove(internals)] {
        });
    });
}

void AsyncFileStream::perform(Function<Function<void(FileStreamClient&)>(FileStream&)>&& operation)
{
    auto& internals = *m_internals;
    callOnFileThread([&internals, operation = WTFMove(operation)] {
        // Don't do the operation if stop was already called on the main thread. Note that there is
        // a race here, but since skipping the operation is an optimization it's OK that we can't
        // guarantee exactly which operations are skipped. Note that this is also the only reason
        // we use an atomic_bool rather than just a bool for destroyed.
        if (internals.destroyed)
            return;
        callOnMainThread([&internals, mainThreadWork = operation(internals.stream)] {
            if (internals.destroyed)
                return;
            mainThreadWork(internals.client);
        });
    });
}

void AsyncFileStream::getSize(const String& path, std::optional<WallTime> expectedModificationTime)
{
    // FIXME: Explicit return type here and in all the other cases like this below is a workaround for a deficiency
    // in the Windows compiler at the time of this writing. Could remove it if that is resolved.
    perform([path = path.isolatedCopy(), expectedModificationTime](FileStream& stream) -> Function<void(FileStreamClient&)> {
        long long size = stream.getSize(path, expectedModificationTime);
        return [size](FileStreamClient& client) {
            client.didGetSize(size);
        };
    });
}

void AsyncFileStream::openForRead(const String& path, long long offset, long long length)
{
    // FIXME: Explicit return type here is a workaround for a deficiency in the Windows compiler at the time of this writing.
    perform([path = path.isolatedCopy(), offset, length](FileStream& stream) -> Function<void(FileStreamClient&)> {
        bool success = stream.openForRead(path, offset, length);
        return [success](FileStreamClient& client) {
            client.didOpen(success);
        };
    });
}

void AsyncFileStream::close()
{
    auto& internals = *m_internals;
    callOnFileThread([&internals] {
        internals.stream.close();
    });
}

void AsyncFileStream::read(void* buffer, int length)
{
    perform([buffer, length](FileStream& stream) -> Function<void(FileStreamClient&)> {
        int bytesRead = stream.read(buffer, length);
        return [bytesRead](FileStreamClient& client) {
            client.didRead(bytesRead);
        };
    });
}

} // namespace WebCore
