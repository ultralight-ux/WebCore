/*
 * Copyright (C) 2016 Konstantin Tokavev <annulen@yandex.ru>
 * Copyright (C) 2016 Yusuke Suzuki <utatane.tea@gmail.com>
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
#include <wtf/RunLoop.h>

#if USE(GLIB)
#include <glib.h>
#include <wtf/glib/RunLoopSourcePriority.h>
#endif

#if USE(ULTRALIGHT)
#include <Ultralight/private/tracy/Tracy.hpp>
#endif

namespace WTF {

#if USE(GLIB)

static const Seconds glibUpdateInterval = 16_ms;

static GSourceFuncs runLoopSourceFunctions = {
    nullptr, // prepare
    nullptr, // check
    // dispatch
    [](GSource* source, GSourceFunc callback, gpointer userData) -> gboolean {
        if (g_source_get_ready_time(source) == -1)
            return G_SOURCE_CONTINUE;
        g_source_set_ready_time(source, -1);
        return callback(userData);
    },
    nullptr, // finalize
    nullptr, // closure_callback
    nullptr, // closure_marshall
};
#endif

class RunLoop::TimerBase::ScheduledTask : public ThreadSafeRefCounted<ScheduledTask> {
WTF_MAKE_NONCOPYABLE(ScheduledTask);
public:
    static Ref<ScheduledTask> create(Function<void()>&& function, Seconds interval, bool repeating)
    {
        return adoptRef(*new ScheduledTask(WTFMove(function), interval, repeating));
    }

    ScheduledTask(Function<void()>&& function, Seconds interval, bool repeating)
        : m_function(WTFMove(function))
        , m_fireInterval(interval)
        , m_isRepeating(repeating)
    {
        updateReadyTime();
    }

    bool fired()
    {
        if (!isActive())
            return false;

        m_function();

        if (!m_isRepeating)
            return false;

        updateReadyTime();
        return isActive();
    }

    MonotonicTime scheduledTimePoint() const
    {
        return m_scheduledTimePoint;
    }

    void updateReadyTime()
    {
        m_scheduledTimePoint = MonotonicTime::now();
        if (!m_fireInterval)
            return;
        m_scheduledTimePoint += m_fireInterval;
    }

    void updateReadyTimeWithThrottledFireInterval(Seconds interval) {
        m_scheduledTimePoint = MonotonicTime::now();
        if (!m_fireInterval)
            return;
        m_scheduledTimePoint += std::max(m_fireInterval, interval);
    }

    struct EarliestSchedule {
        bool operator()(const RefPtr<ScheduledTask>& lhs, const RefPtr<ScheduledTask>& rhs)
        {
            return lhs->scheduledTimePoint() > rhs->scheduledTimePoint();
        }
    };

    bool isActive() const
    {
        return m_isActive.load();
    }

    void deactivate()
    {
        m_isActive.store(false);
    }

private:
    Function<void ()> m_function;
    MonotonicTime m_scheduledTimePoint;
    Seconds m_fireInterval;
    std::atomic<bool> m_isActive { true };
    bool m_isRepeating;
};

RunLoop::RunLoop()
{
#if USE(GLIB)
    m_mainContext = g_main_context_get_thread_default();
    if (!m_mainContext)
        m_mainContext = isMainThread() ? g_main_context_default() : adoptGRef(g_main_context_new());
    ASSERT(m_mainContext);

    GRefPtr<GMainLoop> innermostLoop = adoptGRef(g_main_loop_new(m_mainContext.get(), FALSE));
    ASSERT(innermostLoop);
    m_mainLoopsGlib.append(innermostLoop);

    m_source = adoptGRef(g_source_new(&runLoopSourceFunctions, sizeof(GSource)));
    g_source_set_priority(m_source.get(), RunLoopSourcePriority::RunLoopDispatcher);
    g_source_set_name(m_source.get(), "[WebKit] RunLoop work");
    g_source_set_can_recurse(m_source.get(), TRUE);
    g_source_set_callback(
        m_source.get(), [](gpointer userData) -> gboolean {
            static_cast<RunLoop*>(userData)->performWork();
            return G_SOURCE_CONTINUE;
        },
        this, nullptr);
    g_source_attach(m_source.get(), m_mainContext.get());

    m_lastGlibUpdateTime = MonotonicTime::now();
#endif
}

RunLoop::~RunLoop()
{
    LockHolder locker(m_loopLock);
    m_shutdown = true;
    m_readyToRun.notifyOne();

    // Here is running main loops. Wait until all the main loops are destroyed.
    if (!m_mainLoops.isEmpty())
        m_stopCondition.wait(m_loopLock);

#if USE(GLIB)
    g_source_destroy(m_source.get());

    for (int i = m_mainLoopsGlib.size() - 1; i >= 0; --i) {
        if (!g_main_loop_is_running(m_mainLoopsGlib[i].get()))
            continue;
        g_main_loop_quit(m_mainLoopsGlib[i].get());
    }
#endif
}

inline bool RunLoop::populateTasks(RunMode runMode, Status& statusOfThisLoop, Deque<RefPtr<TimerBase::ScheduledTask>>& firedTimers)
{
    LockHolder locker(m_loopLock);

    if (runMode == RunMode::Drain) {
        MonotonicTime sleepUntil = MonotonicTime::infinity();
        if (!m_schedules.isEmpty())
            sleepUntil = m_schedules.first()->scheduledTimePoint();

        m_readyToRun.waitUntil(m_loopLock, sleepUntil, [&] {
            return m_shutdown || m_pendingTasks || statusOfThisLoop == Status::Stopping;
        });
    }

    if (statusOfThisLoop == Status::Stopping || m_shutdown) {
        m_mainLoops.removeLast();
        if (m_mainLoops.isEmpty())
            m_stopCondition.notifyOne();
        return false;
    }
    m_pendingTasks = false;
    if (runMode == RunMode::Iterate)
        statusOfThisLoop = Status::Stopping;

    // Check expired timers.
    MonotonicTime now = MonotonicTime::now();
    while (!m_schedules.isEmpty()) {
        RefPtr<TimerBase::ScheduledTask> earliest = m_schedules.first();
        if (earliest->scheduledTimePoint() > now)
            break;
        std::pop_heap(m_schedules.begin(), m_schedules.end(), TimerBase::ScheduledTask::EarliestSchedule());
        m_schedules.removeLast();
        firedTimers.append(WTFMove(earliest));
    }

    return true;
}

void RunLoop::runImpl(RunMode runMode)
{
    ASSERT(this == &RunLoop::current());

    Status statusOfThisLoop = Status::Clear;
    {
        LockHolder locker(m_loopLock);
        m_mainLoops.append(&statusOfThisLoop);
    }

    Deque<RefPtr<TimerBase::ScheduledTask>> firedTimers;
    while (true) {
        if (!populateTasks(runMode, statusOfThisLoop, firedTimers))
            return;

        // Dispatch scheduled timers.
        while (!firedTimers.isEmpty()) {
            RefPtr<TimerBase::ScheduledTask> task = firedTimers.takeFirst();
            if (task->fired()) {
                // Reschedule because the timer requires repeating.
                // Since we will query the timers' time points before sleeping,
                // we do not call wakeUp() here.
                schedule(*task);
            }
        }
        performWork();

#if USE(GLIB)
        GMainContext* mainContext = m_mainContext.get();
        g_main_context_push_thread_default(mainContext);
        g_main_context_iteration(mainContext, false);
        g_main_context_pop_thread_default(mainContext);
#endif
    }
}

void RunLoop::run()
{
    RunLoop::current().runImpl(RunMode::Drain);
}

void RunLoop::iterateWithMaxDuration(Seconds duration)
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif

    ASSERT(this == &RunLoop::current());

    MonotonicTime now = MonotonicTime::now();
    MonotonicTime endTime = now + duration;
    do {
        bool didWork = false;

        // Get one task and one function:
        RefPtr<TimerBase::ScheduledTask> nextTask;
        Function<void()> nextFunction;
        {
            LockHolder loopLocker(m_loopLock);
            if (!m_schedules.isEmpty()) {
                RefPtr<TimerBase::ScheduledTask> earliestTask = m_schedules.first();
                if (earliestTask->scheduledTimePoint() <= now) {
                    std::pop_heap(m_schedules.begin(), m_schedules.end(), TimerBase::ScheduledTask::EarliestSchedule());
                    m_schedules.removeLast();
                    nextTask = earliestTask;
                }
            }

            auto locker = holdLock(m_functionQueueLock);
            if (!m_functionQueue.isEmpty())
                nextFunction = m_functionQueue.takeFirst();
        }
    
        // Fire the task
        if (nextTask) {
            MonotonicTime startTime = MonotonicTime::now();
            if (nextTask->fired()) {
                auto taskDuration = MonotonicTime::now() - startTime;
                if (taskDuration > duration) {
                    // We've exceeded our max time budget for this iteration!
                    // Throttle the timer for this task (delay 3 frames) before we reschedule it.
                    nextTask->updateReadyTimeWithThrottledFireInterval(duration * 3.0);
                }
                // Reschedule because the task is repeating.
                schedule(*nextTask);
            }
            didWork = true;
        }

        // Fire the function
        if (nextFunction) {
            nextFunction();
            didWork = true;
        }

        // Bail if we didn't do any work during this iteration.
        if (!didWork)
            break;

    } while (MonotonicTime::now() < endTime);

#if USE(GLIB)
    if (MonotonicTime::now() - m_lastGlibUpdateTime >= glibUpdateInterval) {
        GMainContext* mainContext = m_mainContext.get();
        g_main_context_push_thread_default(mainContext);
        g_main_context_iteration(mainContext, false);
        g_main_context_pop_thread_default(mainContext);
        m_lastGlibUpdateTime = MonotonicTime::now();
    }
#endif
}

void RunLoop::iterate()
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    RunLoop::current().runImpl(RunMode::Iterate);
}

// RunLoop operations are thread-safe. These operations can be called from outside of the RunLoop's thread.
// For example, WorkQueue::{dispatch, dispatchAfter} call the operations of the WorkQueue thread's RunLoop
// from the caller's thread.

void RunLoop::stop()
{
    LockHolder locker(m_loopLock);
    if (m_mainLoops.isEmpty())
        return;

    Status* status = m_mainLoops.last();
    if (*status != Status::Stopping) {
        *status = Status::Stopping;
        m_readyToRun.notifyOne();
    }
}

void RunLoop::wakeUp(const AbstractLocker&)
{
    m_pendingTasks = true;
    m_readyToRun.notifyOne();
}

void RunLoop::wakeUp()
{
    LockHolder locker(m_loopLock);
    wakeUp(locker);
}

void RunLoop::schedule(const AbstractLocker&, Ref<TimerBase::ScheduledTask>&& task)
{
    m_schedules.append(task.ptr());
    std::push_heap(m_schedules.begin(), m_schedules.end(), TimerBase::ScheduledTask::EarliestSchedule());
}

void RunLoop::schedule(Ref<TimerBase::ScheduledTask>&& task)
{
    LockHolder locker(m_loopLock);
    schedule(locker, WTFMove(task));
}

void RunLoop::scheduleAndWakeUp(const AbstractLocker& locker, Ref<TimerBase::ScheduledTask>&& task)
{
    schedule(locker, WTFMove(task));
    wakeUp(locker);
}

void RunLoop::dispatchAfter(Seconds delay, Function<void()>&& function)
{
    LockHolder locker(m_loopLock);
    bool repeating = false;
    schedule(locker, TimerBase::ScheduledTask::create(WTFMove(function), delay, repeating));
    wakeUp(locker);
}

// Since RunLoop does not own the registered TimerBase,
// TimerBase and its owner should manage these lifetime.
RunLoop::TimerBase::TimerBase(RunLoop& runLoop)
    : m_runLoop(runLoop)
    , m_scheduledTask(nullptr)
{
}

RunLoop::TimerBase::~TimerBase()
{
    LockHolder locker(m_runLoop->m_loopLock);
    stop(locker);
}

void RunLoop::TimerBase::start(Seconds interval, bool repeating)
{
    LockHolder locker(m_runLoop->m_loopLock);
    stop(locker);
    m_scheduledTask = ScheduledTask::create([this] {
        fired();
    }, interval, repeating);
    m_runLoop->scheduleAndWakeUp(locker, *m_scheduledTask);
}

void RunLoop::TimerBase::stop(const AbstractLocker&)
{
    if (m_scheduledTask) {
        m_scheduledTask->deactivate();
        m_scheduledTask = nullptr;
    }
}

void RunLoop::TimerBase::stop()
{
    LockHolder locker(m_runLoop->m_loopLock);
    stop(locker);
}

bool RunLoop::TimerBase::isActive() const
{
    LockHolder locker(m_runLoop->m_loopLock);
    return isActive(locker);
}

bool RunLoop::TimerBase::isActive(const AbstractLocker&) const
{
    return m_scheduledTask;
}

Seconds RunLoop::TimerBase::secondsUntilFire() const
{
    LockHolder locker(m_runLoop->m_loopLock);
    if (isActive(locker))
        return std::max<Seconds>(m_scheduledTask->scheduledTimePoint() - MonotonicTime::now(), 0_s);
    return 0_s;
}

} // namespace WTF
