// Copyright Ultralight, Inc. 2022. All rights reserved.
#pragma once
#include <wtf/ExportMacros.h>
#include <wtf/Function.h>

namespace WTF {

// This is the order by which shutdown functions will be called.
// Higher priority will be called first inside WTF::Shutdown().
enum class ShutdownPriority : uint8_t {
    Last,      // Library and support frameworks
    Low,
    Normal,
    High,
    High_1,
    High_2,
    High_3,
    High_4,
    High_5,    
    Highest    // Active executions (eg, JavaScript and network fetches)
};

// Register a function to be called during WTF::Shutdown with an optional priority.
// This can be used to cleanup singletons and threads.
WTF_EXPORT_PRIVATE void CallOnShutdown(WTF::Function<void()>&& func, ShutdownPriority priority = ShutdownPriority::Normal);

// This should be called at library shutdown.
WTF_EXPORT_PRIVATE void Shutdown();

} // namespace WTF
