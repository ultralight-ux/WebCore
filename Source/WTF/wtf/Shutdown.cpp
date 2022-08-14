// Copyright Ultralight, Inc. 2022. All rights reserved.
#include "config.h"
#include <wtf/Shutdown.h>
#include <wtf/Deque.h>
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
}

}  // namespace WTF
