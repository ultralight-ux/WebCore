// Copyright Ultralight, Inc. 2022. All rights reserved.
#include "config.h"
#include <wtf/Shutdown.h>
#include <wtf/Deque.h>
#include <wtf/ThreadSpecific.h>
#include <memory>

namespace WTF {

struct ShutdownTask {
    ShutdownTask(WTF::Function<void()>&& func)
        : m_func(WTFMove(func))
    {
    }

    WTF::Function<void()> m_func;
};

static WTF::Deque<std::unique_ptr<ShutdownTask>> shutdownTasks;

void CallOnShutdown(WTF::Function<void()>&& func)
{
    shutdownTasks.append(std::make_unique<ShutdownTask>(WTFMove(func)));
}

void Shutdown() {
    while (!shutdownTasks.isEmpty())
    {
        auto lastTask = shutdownTasks.takeLast();
        lastTask->m_func();
    }

#if OS(WINDOWS)
    // Give threads a chance to exit
    Sleep(100);

    // Clear out the FLS values (to prevent destruction callbacks from being invoked)
    // and return FLS keys to the process
    flsKeyDestroyAll();
#endif
}

}  // namespace WTF
