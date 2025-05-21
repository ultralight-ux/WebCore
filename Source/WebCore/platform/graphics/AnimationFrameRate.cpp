/*
 * Copyright (C) 2020 Apple Inc. All rights reserved.
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
#include "AnimationFrameRate.h"
#include <wtf/text/TextStream.h>

#if USE(ULTRALIGHT)
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <cmath>
#endif

namespace WebCore {

static constexpr OptionSet<ThrottlingReason> halfSpeedThrottlingReasons { ThrottlingReason::LowPowerMode, ThrottlingReason::NonInteractedCrossOriginFrame, ThrottlingReason::VisuallyIdle };

FramesPerSecond getFullSpeedFramesPerSecond()
{
#if USE(ULTRALIGHT)
    double animation_interval = ultralight::Platform::instance().config().animation_timer_delay;
    if (animation_interval > 0)
        return static_cast<unsigned>(std::ceil(1.0 / animation_interval));
#endif
    return FullSpeedFramesPerSecond;
}

Seconds getFullSpeedAnimationInterval()
{
#if USE(ULTRALIGHT)
    double animation_interval = ultralight::Platform::instance().config().animation_timer_delay;
    if (animation_interval > 0)
        return Seconds(animation_interval);
#endif
    return FullSpeedAnimationInterval;
}

FramesPerSecond framesPerSecondNearestFullSpeed(FramesPerSecond nominalFramesPerSecond)
{
    if (nominalFramesPerSecond <= getFullSpeedFramesPerSecond())
        return nominalFramesPerSecond;

    float fullSpeedRatio = nominalFramesPerSecond / getFullSpeedFramesPerSecond();
    FramesPerSecond floorSpeed = nominalFramesPerSecond / std::floor(fullSpeedRatio);
    FramesPerSecond ceilSpeed = nominalFramesPerSecond / std::ceil(fullSpeedRatio);

    return fullSpeedRatio - std::floor(fullSpeedRatio) <= 0.5 ? floorSpeed : ceilSpeed;
}

std::optional<FramesPerSecond> preferredFramesPerSecond(OptionSet<ThrottlingReason> reasons, std::optional<FramesPerSecond> nominalFramesPerSecond, bool preferFrameRatesNear60FPS)
{
    if (reasons.contains(ThrottlingReason::OutsideViewport))
        return std::nullopt;

    if (!nominalFramesPerSecond || *nominalFramesPerSecond == getFullSpeedFramesPerSecond()) {
        // FIXME: handle ThrottlingReason::VisuallyIdle
        if (reasons.containsAny(halfSpeedThrottlingReasons))
            return HalfSpeedThrottlingFramesPerSecond;

        return getFullSpeedFramesPerSecond();
    }

    auto framesPerSecond = preferFrameRatesNear60FPS ? framesPerSecondNearestFullSpeed(*nominalFramesPerSecond) : *nominalFramesPerSecond;
    if (reasons.containsAny(halfSpeedThrottlingReasons))
        framesPerSecond /= IntervalThrottlingFactor;

    return framesPerSecond;
}

Seconds preferredFrameInterval(OptionSet<ThrottlingReason> reasons, std::optional<FramesPerSecond> nominalFramesPerSecond, bool preferFrameRatesNear60FPS)
{
    if (reasons.contains(ThrottlingReason::OutsideViewport))
        return AggressiveThrottlingAnimationInterval;

    if (!nominalFramesPerSecond || *nominalFramesPerSecond == getFullSpeedFramesPerSecond()) {
        // FIXME: handle ThrottlingReason::VisuallyIdle
        if (reasons.containsAny(halfSpeedThrottlingReasons))
            return HalfSpeedThrottlingAnimationInterval;
        return getFullSpeedAnimationInterval();
    }

    auto framesPerSecond = preferFrameRatesNear60FPS ? framesPerSecondNearestFullSpeed(*nominalFramesPerSecond) : *nominalFramesPerSecond;
    auto interval = Seconds(1.0 / framesPerSecond);

    if (reasons.containsAny(halfSpeedThrottlingReasons))
        interval *= IntervalThrottlingFactor;

    return interval;
}

FramesPerSecond preferredFramesPerSecondFromInterval(Seconds preferredFrameInterval)
{
    if (preferredFrameInterval == getFullSpeedAnimationInterval())
        return getFullSpeedFramesPerSecond();

    if (preferredFrameInterval == HalfSpeedThrottlingAnimationInterval)
        return HalfSpeedThrottlingFramesPerSecond;

    return std::round(1 / preferredFrameInterval.seconds());
}

TextStream& operator<<(TextStream& ts, const OptionSet<ThrottlingReason>& reasons)
{
    bool didAppend = false;

    for (auto reason : reasons) {
        if (didAppend)
            ts << "|";
        switch (reason) {
        case ThrottlingReason::VisuallyIdle:
            ts << "VisuallyIdle";
            break;
        case ThrottlingReason::OutsideViewport:
            ts << "OutsideViewport";
            break;
        case ThrottlingReason::LowPowerMode:
            ts << "LowPowerMode";
            break;
        case ThrottlingReason::NonInteractedCrossOriginFrame:
            ts << "NonInteractiveCrossOriginFrame";
            break;
        }
        didAppend = true;
    }

    if (reasons.isEmpty())
        ts << "[Unthrottled]";
    return ts;
}
} // namespace WebCore
