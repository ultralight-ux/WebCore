/*
 * Copyright (c) 2016 Igalia S.L.
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
#include "ScrollAnimatorGeneric.h"

#include "ScrollAnimationKinetic.h"
#include "ScrollAnimationSmooth.h"
#include "ScrollableArea.h"
#include "ScrollbarTheme.h"

#if USE(ULTRALIGHT)
#include <Ultralight/private/tracy/Tracy.hpp>
#endif

namespace WebCore {

static const Seconds overflowScrollbarsAnimationDuration { 1_s };
static const Seconds overflowScrollbarsAnimationHideDelay { 2_s };
static const Seconds scrollCaptureThreshold { 150_ms };

std::unique_ptr<ScrollAnimator> ScrollAnimator::create(ScrollableArea& scrollableArea)
{
    return std::make_unique<ScrollAnimatorGeneric>(scrollableArea);
}

ScrollAnimatorGeneric::ScrollAnimatorGeneric(ScrollableArea& scrollableArea)
    : ScrollAnimator(scrollableArea)
    , m_overlayScrollbarAnimationTimer(*this, &ScrollAnimatorGeneric::overlayScrollbarAnimationTimerFired)
{
    m_kineticAnimation = std::make_unique<ScrollAnimationKinetic>(m_scrollableArea, [this](FloatPoint&& position) {
#if ENABLE(SMOOTH_SCROLLING)
        if (m_smoothAnimation)
            m_smoothAnimation->setCurrentPosition(position);
#endif
        updatePosition(WTFMove(position));
    });

#if ENABLE(SMOOTH_SCROLLING)
    if (scrollableArea.scrollAnimatorEnabled())
        ensureSmoothScrollingAnimation();
#endif
}

ScrollAnimatorGeneric::~ScrollAnimatorGeneric() = default;

#if ENABLE(SMOOTH_SCROLLING)
void ScrollAnimatorGeneric::ensureSmoothScrollingAnimation()
{
    if (m_smoothAnimation)
        return;

    m_smoothAnimation = std::make_unique<ScrollAnimationSmooth>(m_scrollableArea, m_currentPosition, [this](FloatPoint&& position) {
        updatePosition(WTFMove(position));
    });
}
#endif

#if ENABLE(SMOOTH_SCROLLING)
bool ScrollAnimatorGeneric::scroll(ScrollbarOrientation orientation, ScrollGranularity granularity, float step, float multiplier)
{
    if (!m_scrollableArea.scrollAnimatorEnabled())
        return ScrollAnimator::scroll(orientation, granularity, step, multiplier);

    ensureSmoothScrollingAnimation();
    return m_smoothAnimation->scroll(orientation, granularity, step, multiplier);
}
#endif

void ScrollAnimatorGeneric::scrollToOffsetWithoutAnimation(const FloatPoint& offset, ScrollClamping)
{
    FloatPoint position = ScrollableArea::scrollPositionFromOffset(offset, toFloatSize(m_scrollableArea.scrollOrigin()));
    m_kineticAnimation->stop();
    m_scrollHistory.clear();

#if ENABLE(SMOOTH_SCROLLING)
    if (m_smoothAnimation)
        m_smoothAnimation->setCurrentPosition(position);
#endif

    updatePosition(WTFMove(position));
}

FloatPoint ScrollAnimatorGeneric::computeVelocity()
{
    if (m_scrollHistory.isEmpty())
        return { };

    auto first = m_scrollHistory[0].timestamp();
    auto last = m_scrollHistory.rbegin()->timestamp();

    if (last == first)
        return { };

    FloatPoint accumDelta;
    for (const auto& scrollEvent : m_scrollHistory)
        accumDelta += FloatPoint(scrollEvent.deltaX(), scrollEvent.deltaY());

    m_scrollHistory.clear();

    return FloatPoint(accumDelta.x() * -1 / (last - first).value(), accumDelta.y() * -1 / (last - first).value());
}

bool ScrollAnimatorGeneric::handleWheelEvent(const PlatformWheelEvent& event)
{
    m_kineticAnimation->stop();

    m_scrollHistory.removeAllMatching([&event] (PlatformWheelEvent& otherEvent) -> bool {
        return (event.timestamp() - otherEvent.timestamp()) > scrollCaptureThreshold;
    });

#if ENABLE(ASYNC_SCROLLING)
    if (event.isEndOfNonMomentumScroll()) {
        // We don't need to add the event to the history as its delta will be (0, 0).
        static_cast<ScrollAnimationKinetic*>(m_kineticAnimation.get())->start(m_currentPosition, computeVelocity(), m_scrollableArea.horizontalScrollbar(), m_scrollableArea.verticalScrollbar());
        return true;
    }
    if (event.isTransitioningToMomentumScroll()) {
        m_scrollHistory.clear();
        static_cast<ScrollAnimationKinetic*>(m_kineticAnimation.get())->start(m_currentPosition, event.swipeVelocity(), m_scrollableArea.horizontalScrollbar(), m_scrollableArea.verticalScrollbar());
        return true;
    }
#endif

    m_scrollHistory.append(event);

    return ScrollAnimator::handleWheelEvent(event);
}

void ScrollAnimatorGeneric::willEndLiveResize()
{
    m_kineticAnimation->updateVisibleLengths();

#if ENABLE(SMOOTH_SCROLLING)
    if (m_smoothAnimation)
        m_smoothAnimation->updateVisibleLengths();
#endif
}

void ScrollAnimatorGeneric::updatePosition(FloatPoint&& position)
{
    FloatSize delta = position - m_currentPosition;
    m_currentPosition = WTFMove(position);
    notifyPositionChanged(delta);
}

void ScrollAnimatorGeneric::didAddVerticalScrollbar(Scrollbar* scrollbar)
{
    m_kineticAnimation->updateVisibleLengths();

#if ENABLE(SMOOTH_SCROLLING)
    if (m_smoothAnimation)
        m_smoothAnimation->updateVisibleLengths();
#endif
    if (!scrollbar->isOverlayScrollbar())
        return;
    m_verticalOverlayScrollbar = scrollbar;
    if (!m_horizontalOverlayScrollbar)
        m_overlayScrollbarAnimationCurrent = 1;
    m_verticalOverlayScrollbar->setOpacity(m_overlayScrollbarAnimationCurrent);
    hideOverlayScrollbars();
}

void ScrollAnimatorGeneric::didAddHorizontalScrollbar(Scrollbar* scrollbar)
{
    m_kineticAnimation->updateVisibleLengths();

#if ENABLE(SMOOTH_SCROLLING)
    if (m_smoothAnimation)
        m_smoothAnimation->updateVisibleLengths();
#endif
    if (!scrollbar->isOverlayScrollbar())
        return;
    m_horizontalOverlayScrollbar = scrollbar;
    if (!m_verticalOverlayScrollbar)
        m_overlayScrollbarAnimationCurrent = 1;
    m_horizontalOverlayScrollbar->setOpacity(m_overlayScrollbarAnimationCurrent);
    hideOverlayScrollbars();
}

void ScrollAnimatorGeneric::willRemoveVerticalScrollbar(Scrollbar* scrollbar)
{
    if (m_verticalOverlayScrollbar != scrollbar)
        return;
    m_verticalOverlayScrollbar = nullptr;
    if (!m_horizontalOverlayScrollbar)
        m_overlayScrollbarAnimationCurrent = 0;
}

void ScrollAnimatorGeneric::willRemoveHorizontalScrollbar(Scrollbar* scrollbar)
{
    if (m_horizontalOverlayScrollbar != scrollbar)
        return;
    m_horizontalOverlayScrollbar = nullptr;
    if (!m_verticalOverlayScrollbar)
        m_overlayScrollbarAnimationCurrent = 0;
}

void ScrollAnimatorGeneric::updateOverlayScrollbarsOpacity()
{
    if (m_verticalOverlayScrollbar && m_overlayScrollbarAnimationCurrent != m_verticalOverlayScrollbar->opacity()) {
        m_verticalOverlayScrollbar->setOpacity(m_overlayScrollbarAnimationCurrent);
        if (m_verticalOverlayScrollbar->hoveredPart() == NoPart)
            m_verticalOverlayScrollbar->invalidate();
    }

    if (m_horizontalOverlayScrollbar && m_overlayScrollbarAnimationCurrent != m_horizontalOverlayScrollbar->opacity()) {
        m_horizontalOverlayScrollbar->setOpacity(m_overlayScrollbarAnimationCurrent);
        if (m_horizontalOverlayScrollbar->hoveredPart() == NoPart)
            m_horizontalOverlayScrollbar->invalidate();
    }
}

static inline double easeOutCubic(double t)
{
    double p = t - 1;
    return p * p * p + 1;
}

void ScrollAnimatorGeneric::overlayScrollbarAnimationTimerFired()
{
#if USE(ULTRALIGHT)
    ProfiledZone;
#endif
    if (!m_horizontalOverlayScrollbar && !m_verticalOverlayScrollbar)
        return;
    if (m_overlayScrollbarsLocked)
        return;

    MonotonicTime currentTime = MonotonicTime::now();
    double progress = 1;
    if (currentTime < m_overlayScrollbarAnimationEndTime)
        progress = (currentTime - m_overlayScrollbarAnimationStartTime).value() / (m_overlayScrollbarAnimationEndTime - m_overlayScrollbarAnimationStartTime).value();
    progress = m_overlayScrollbarAnimationSource + (easeOutCubic(progress) * (m_overlayScrollbarAnimationTarget - m_overlayScrollbarAnimationSource));
    if (progress != m_overlayScrollbarAnimationCurrent) {
        m_overlayScrollbarAnimationCurrent = progress;
        updateOverlayScrollbarsOpacity();
    }

    if (m_overlayScrollbarAnimationCurrent != m_overlayScrollbarAnimationTarget) {
        static const double frameRate = 120;
        static const Seconds tickTime = 1_s / frameRate;
        static const Seconds minimumTimerInterval = 1_ms;
        Seconds deltaToNextFrame = std::max(tickTime - (MonotonicTime::now() - currentTime), minimumTimerInterval);
        m_overlayScrollbarAnimationTimer.startOneShot(deltaToNextFrame);
    } else
        hideOverlayScrollbars();
}

void ScrollAnimatorGeneric::showOverlayScrollbars()
{
    if (m_overlayScrollbarsLocked)
        return;

    if (m_overlayScrollbarAnimationTimer.isActive() && m_overlayScrollbarAnimationTarget == 1)
        return;
    m_overlayScrollbarAnimationTimer.stop();

    if (!m_horizontalOverlayScrollbar && !m_verticalOverlayScrollbar)
        return;

    m_overlayScrollbarAnimationSource = m_overlayScrollbarAnimationCurrent;
    m_overlayScrollbarAnimationTarget = 1;
    if (m_overlayScrollbarAnimationTarget != m_overlayScrollbarAnimationCurrent) {
        m_overlayScrollbarAnimationStartTime = MonotonicTime::now();
        m_overlayScrollbarAnimationEndTime = m_overlayScrollbarAnimationStartTime + overflowScrollbarsAnimationDuration;
        m_overlayScrollbarAnimationTimer.startOneShot(0_s);
    } else
        hideOverlayScrollbars();
}

void ScrollAnimatorGeneric::hideOverlayScrollbars()
{
    if (m_overlayScrollbarAnimationTimer.isActive() && !m_overlayScrollbarAnimationTarget)
        return;
    m_overlayScrollbarAnimationTimer.stop();

    if (!m_horizontalOverlayScrollbar && !m_verticalOverlayScrollbar)
        return;

    m_overlayScrollbarAnimationSource = m_overlayScrollbarAnimationCurrent;
    m_overlayScrollbarAnimationTarget = 0;
    if (m_overlayScrollbarAnimationTarget == m_overlayScrollbarAnimationCurrent)
        return;
    m_overlayScrollbarAnimationStartTime = MonotonicTime::now() + overflowScrollbarsAnimationHideDelay;
    m_overlayScrollbarAnimationEndTime = m_overlayScrollbarAnimationStartTime + overflowScrollbarsAnimationDuration + overflowScrollbarsAnimationHideDelay;
    m_overlayScrollbarAnimationTimer.startOneShot(overflowScrollbarsAnimationHideDelay);
}

void ScrollAnimatorGeneric::mouseEnteredContentArea()
{
    showOverlayScrollbars();
}

void ScrollAnimatorGeneric::mouseExitedContentArea()
{
    hideOverlayScrollbars();
}

void ScrollAnimatorGeneric::mouseMovedInContentArea()
{
    showOverlayScrollbars();
}

void ScrollAnimatorGeneric::contentAreaDidShow()
{
    showOverlayScrollbars();
}

void ScrollAnimatorGeneric::contentAreaDidHide()
{
    if (m_overlayScrollbarsLocked)
        return;
    m_overlayScrollbarAnimationTimer.stop();
    if (m_overlayScrollbarAnimationCurrent) {
        m_overlayScrollbarAnimationCurrent = 0;
        updateOverlayScrollbarsOpacity();
    }
}

void ScrollAnimatorGeneric::notifyContentAreaScrolled(const FloatSize&)
{
    showOverlayScrollbars();
}

void ScrollAnimatorGeneric::lockOverlayScrollbarStateToHidden(bool shouldLockState)
{
    if (m_overlayScrollbarsLocked == shouldLockState)
        return;
    m_overlayScrollbarsLocked = shouldLockState;

    if (!m_horizontalOverlayScrollbar && !m_verticalOverlayScrollbar)
        return;

    if (m_overlayScrollbarsLocked) {
        m_overlayScrollbarAnimationTimer.stop();
        if (m_horizontalOverlayScrollbar)
            m_horizontalOverlayScrollbar->setOpacity(0);
        if (m_verticalOverlayScrollbar)
            m_verticalOverlayScrollbar->setOpacity(0);
    } else {
        if (m_overlayScrollbarAnimationCurrent == 1)
            updateOverlayScrollbarsOpacity();
        else
            showOverlayScrollbars();
    }
}

} // namespace WebCore

