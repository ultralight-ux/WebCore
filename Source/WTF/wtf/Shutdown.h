// Copyright Ultralight, Inc. 2022. All rights reserved.
#pragma once
#include <wtf/Function.h>
#include <wtf/ExportMacros.h>

namespace WTF {

WTF_EXPORT_PRIVATE void CallOnShutdown(WTF::Function<void()>&& func);

WTF_EXPORT_PRIVATE void Shutdown();

}  // namespace WTF
