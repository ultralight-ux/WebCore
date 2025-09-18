/*
 * Copyright (C) 2016 Igalia S.L.
 * Copyright (C) 2015-2021 Apple Inc. All rights reserved.
 * Copyright (c) 2011, Google Inc. All rights reserved.
 * Copyright (C) 2025 Ultralight, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ScrollAnimationSmooth.h"

#include "FloatPoint.h"
#include "GeometryUtilities.h"
#include "ScrollExtents.h"
#include "ScrollableArea.h"
#include "TimingFunction.h"
#include <wtf/text/TextStream.h>

namespace WebCore {

/////////////////////////////////////////////////////////////////////////////////////////
// 
//    This file implements smooth scrolling as a 1-D spring-mass system with
//    *critical damping*. The main motivation is to have a smooth, snappy scrolling
//    experience, without overshoot, oscillation, nor visual discontinuities in the first
//    derivative (even when we receive a bunch of scroll events at once).
//
//    1.  Equation of motion (each axis is solved independently):
//            d2x/dt2 + 2*zeta*omega * dx/dt + omega^2 * (x - target) = 0
//
//        x           current scroll offset   (pixels)
//        target      destination offset      (pixels)
//        dx/dt       velocity                (pixels / ms)
//        d2x/dt2     acceleration            (pixels / ms^2)
//        omega       natural frequency       (rad / ms)  – "stiffness"
//        zeta        damping ratio           (unitless)
//                    zeta = 1.0  -> critical damping (fastest settle, no overshoot)
//
//    2.  Integration (per animation frame)
//            v += a * dt        // update velocity   (semi-implicit Euler)
//            x += v * dt        // update position
//
//        This scheme is stable even with large or varying dt values.
//
//    3.  Retargeting policy
//        retargetActiveAnimation() only changes the destination.  Velocity is
//        preserved, so the curve remains C1-continuous (no sudden jump).
//
//    4.  Stop conditions
//        Animation ends when BOTH position and velocity are almost zero.
// 
//    5.  Tunable constants:
//          omega*        0.025f   // higher => snappier, shorter tail
//          zeta          1.0f     // >=1.0 avoids overshoot (use < 1.0 to make it springy)
// 
//          * omega was chosen to approximate the 0.998 deceleration rate in iOS ScrollView.
// 
/////////////////////////////////////////////////////////////////////////////////////////

#if OS(DARWIN)
// We use a slightly stiffer spring constant on iOS/macOS because the OS already has some
// inherent smoothing in its scroll events and we don't want to overdo it.
static constexpr float omega = 0.025f;      // Stiffness of spring (see above)
#else
static constexpr float omega = 0.022f;      // Stiffness of spring (see above)
#endif
static constexpr float zeta = 1.0f;         // Critical damping (see above)

ScrollAnimationSmooth::ScrollAnimationSmooth(ScrollAnimationClient& client)
    : ScrollAnimation(Type::Smooth, client)
{
}

ScrollAnimationSmooth::~ScrollAnimationSmooth() = default;

bool ScrollAnimationSmooth::startAnimatedScrollToDestination(const FloatPoint& from,
    const FloatPoint& to)
{
    auto ext = m_client.scrollExtentsForAnimation(*this);
    m_currentOffset = from;
    m_destinationOffset = to.constrainedBetween(ext.minimumScrollOffset(),
        ext.maximumScrollOffset());

    if (!isActive() && from == m_destinationOffset)
        return false; // already there

    m_velocity = { 0, 0 }; // we start at rest, imagine pulling back a spring...
    m_lastTime = MonotonicTime::now();

    if (!isActive())
        didStart(*m_lastTime);

    return true;
}

// This is called when there is already an active animation.
bool ScrollAnimationSmooth::retargetActiveAnimation(const FloatPoint& newDest)
{
    if (!isActive())
        return false;

    auto ext = m_client.scrollExtentsForAnimation(*this);
    m_destinationOffset = newDest.constrainedBetween(ext.minimumScrollOffset(),
        ext.maximumScrollOffset());

    if (m_destinationOffset == m_currentOffset)
        return false;

    // Velocity is left untouched (continuously integrated in animateScroll())

    return true;
}

void ScrollAnimationSmooth::updateScrollExtents()
{
    auto extents = m_client.scrollExtentsForAnimation(*this);
    m_destinationOffset = m_destinationOffset.constrainedBetween(extents.minimumScrollOffset(), extents.maximumScrollOffset());
}

void ScrollAnimationSmooth::serviceAnimation(MonotonicTime currentTime)
{
    bool animationActive = animateScroll(currentTime);
    m_client.scrollAnimationDidUpdate(*this, m_currentOffset);
    if (!animationActive)
        didEnd();
}

bool ScrollAnimationSmooth::animateScroll(MonotonicTime now)
{
    if (!m_lastTime)
        m_lastTime = now;

    float elapsedMs = static_cast<float>((now - *m_lastTime).milliseconds());
    if (elapsedMs <= 0)
        return true; // no time traveling

    // Fixed timestep physics to handle variable frame rates
    const float fixedStepMs = 4.0f;  // 4ms = 250Hz physics
    const int maxIterations = 400;   // Safety cap: 400 * 4ms = 1.6 seconds

    // Track displacement before physics updates
    FloatSize dispBefore = m_currentOffset - m_destinationOffset;

    // Process all elapsed time in fixed steps
    int iterations = 0;
    while (elapsedMs > 0 && iterations < maxIterations) {
        float dt = std::min(elapsedMs, fixedStepMs);

        // 1. spring acceleration (critically-damped)
        FloatSize disp = m_currentOffset - m_destinationOffset;

        float ax = -omega * omega * disp.width()
            - 2.0f * zeta * omega * m_velocity.x();
        float ay = -omega * omega * disp.height()
            - 2.0f * zeta * omega * m_velocity.y();

        // 2. integrate velocity
        m_velocity.setX(m_velocity.x() + ax * dt);
        m_velocity.setY(m_velocity.y() + ay * dt);

        // 3. integrate position
        m_currentOffset.move(m_velocity.x() * dt,
            m_velocity.y() * dt);

        elapsedMs -= dt;
        iterations++;
    }

    // 4. test if we should stop (we overshot or are close enough)
    FloatSize dispAfter = m_currentOffset - m_destinationOffset; // new displacement (in px)

    auto signFlip = [](float a, float b) {
        return (a > 0.f) != (b > 0.f);
    };

    bool overshootX = signFlip(dispBefore.width(), dispAfter.width());
    bool overshootY = signFlip(dispBefore.height(), dispAfter.height());

    constexpr float posEps = 1.0f; // px
    constexpr float velEps = 0.02f; // px / ms

    bool inTolerance = std::fabs(dispAfter.width()) <= posEps && 
        std::fabs(dispAfter.height()) <= posEps &&
        std::fabs(m_velocity.x()) <= velEps &&
        std::fabs(m_velocity.y()) <= velEps;

    if (overshootX || overshootY || inTolerance) {
        m_currentOffset = m_destinationOffset; // snap to destination
        return false; // stop animating
    }

    m_lastTime = now;
    return true; // continue animating
}

String ScrollAnimationSmooth::debugDescription() const
{
    TextStream textStream;
    return textStream.release();
}

} // namespace WebCore