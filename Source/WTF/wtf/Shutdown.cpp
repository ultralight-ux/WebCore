// Copyright Ultralight, Inc. 2022. All rights reserved.
#include "config.h"
#include <wtf/Shutdown.h>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>
#include <wtf/ThreadSpecific.h>
#include <memory>

namespace WTF {

struct ShutdownTask {
    ShutdownTask(WTF::Function<void()>&& func, ShutdownPriority priority)
        : m_func(WTFMove(func))
        , m_priority(priority)
    {
    }

    WTF::Function<void()> m_func;
    ShutdownPriority m_priority;
};

typedef std::unique_ptr<ShutdownTask> TaskPtr;

class TaskPtrCompare {
public:
    bool operator()(const TaskPtr& a, const TaskPtr& b)
    {
        return a->m_priority < b->m_priority;
    }
};

static std::priority_queue<TaskPtr, std::vector<TaskPtr>, TaskPtrCompare> g_shutdownTasks;
static std::mutex g_shutdownTasksMutex;

void CallOnShutdown(WTF::Function<void()>&& func, ShutdownPriority priority)
{
    std::lock_guard<std::mutex> guard(g_shutdownTasksMutex);
    g_shutdownTasks.push(std::make_unique<ShutdownTask>(WTFMove(func), priority));
}

void Shutdown() {

    do
    {
        std::unique_ptr<ShutdownTask> nextTask;

        {
            std::lock_guard<std::mutex> guard(g_shutdownTasksMutex);
            if (g_shutdownTasks.empty())
                break;

            nextTask = std::move(const_cast<TaskPtr&>(g_shutdownTasks.top()));
            g_shutdownTasks.pop();
        }

        // The ShutdownTask could call CallOnShutdown() with another task so we have to make sure
        // we aren't holding the lock for the priority_queue before dispatching the task.
        nextTask->m_func();
    } while (true);

#if OS(WINDOWS)
    // Give threads a chance to exit
    Sleep(100);

    // Clear out the FLS values (to prevent destruction callbacks from being invoked)
    // and return FLS keys to the process
    flsKeyDestroyAll();
#endif
}

}  // namespace WTF
