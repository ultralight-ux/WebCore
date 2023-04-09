/*
 * Copyright (c) 2009, Google Inc. All rights reserved.
 * Copyright (C) 2020, 2021, 2022 Igalia S.L.
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
#include "RenderSVGModelObject.h"

#if ENABLE(LAYER_BASED_SVG_ENGINE)
#include "RenderGeometryMap.h"
#include "RenderLayer.h"
#include "RenderLayerModelObject.h"
#include "RenderSVGModelObjectInlines.h"
#include "RenderSVGResource.h"
#include "RenderView.h"
#include "SVGElementInlines.h"
#include "SVGGraphicsElement.h"
#include "SVGLocatable.h"
#include "SVGNames.h"
#include "SVGPathData.h"
#include "SVGResourcesCache.h"
#include "TransformState.h"
#include <wtf/IsoMallocInlines.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(RenderSVGModelObject);

RenderSVGModelObject::RenderSVGModelObject(Document& document, RenderStyle&& style)
    : RenderLayerModelObject(document, WTFMove(style), 0)
{
}

RenderSVGModelObject::RenderSVGModelObject(SVGElement& element, RenderStyle&& style)
    : RenderLayerModelObject(element, WTFMove(style), 0)
{
}

void RenderSVGModelObject::updateFromStyle()
{
    RenderLayerModelObject::updateFromStyle();
    updateHasSVGTransformFlags();
}

LayoutRect RenderSVGModelObject::overflowClipRect(const LayoutPoint&, RenderFragmentContainer*, OverlayScrollbarSizeRelevancy, PaintPhase) const
{
    ASSERT_NOT_REACHED();
    return LayoutRect();
}

LayoutRect RenderSVGModelObject::clippedOverflowRect(const RenderLayerModelObject* repaintContainer, VisibleRectContext context) const
{
    if (isInsideEntirelyHiddenLayer())
        return { };

    ASSERT(!view().frameView().layoutContext().isPaintOffsetCacheEnabled());
    return computeRect(visualOverflowRectEquivalent(), repaintContainer, context);
}

std::optional<LayoutRect> RenderSVGModelObject::computeVisibleRectInContainer(const LayoutRect& rect, const RenderLayerModelObject* container, VisibleRectContext context) const
{
    return computeVisibleRectInSVGContainer(rect, container, context);
}

const RenderObject* RenderSVGModelObject::pushMappingToContainer(const RenderLayerModelObject* ancestorToStopAt, RenderGeometryMap& geometryMap) const
{
    ASSERT(ancestorToStopAt != this);
    ASSERT(style().position() == PositionType::Static);

    bool ancestorSkipped;
    RenderElement* container = this->container(ancestorToStopAt, ancestorSkipped);
    if (!container)
        return nullptr;

    ASSERT_UNUSED(ancestorSkipped, !ancestorSkipped);

    pushOntoGeometryMap(geometryMap, ancestorToStopAt, container, ancestorSkipped);
    return container;
}

LayoutRect RenderSVGModelObject::outlineBoundsForRepaint(const RenderLayerModelObject* repaintContainer, const RenderGeometryMap* geometryMap) const
{
    ASSERT(!view().frameView().layoutContext().isPaintOffsetCacheEnabled());

    auto outlineBounds = visualOverflowRectEquivalent();

    if (repaintContainer != this) {
        FloatQuad containerRelativeQuad;
        if (geometryMap)
            containerRelativeQuad = geometryMap->mapToContainer(outlineBounds, repaintContainer);
        else
            containerRelativeQuad = localToContainerQuad(FloatRect(outlineBounds), repaintContainer);

        outlineBounds = LayoutRect(containerRelativeQuad.boundingBox());
    }

    return outlineBounds;
}

void RenderSVGModelObject::absoluteRects(Vector<IntRect>& rects, const LayoutPoint& accumulatedOffset) const
{
    rects.append(snappedIntRect(accumulatedOffset, m_layoutRect.size()));
}

void RenderSVGModelObject::absoluteQuads(Vector<FloatQuad>& quads, bool* wasFixed) const
{
    quads.append(localToAbsoluteQuad(FloatRect { { }, m_layoutRect.size() }, UseTransforms, wasFixed));
}

void RenderSVGModelObject::willBeDestroyed()
{
    SVGResourcesCache::clientDestroyed(*this);
    RenderLayerModelObject::willBeDestroyed();
}

void RenderSVGModelObject::styleDidChange(StyleDifference diff, const RenderStyle* oldStyle)
{
    RenderLayerModelObject::styleDidChange(diff, oldStyle);

    // SVG masks are painted independent of the target renderers visibility.
    // FIXME: [LBSE] Upstream RenderElement changes
    // bool hasSVGMask = hasSVGMask();
    bool hasSVGMask = false;
    if (hasSVGMask && hasLayer() && style().visibility() != Visibility::Visible)
        layer()->setHasVisibleContent();

    SVGResourcesCache::clientStyleChanged(*this, diff, oldStyle, style());
}

void RenderSVGModelObject::mapAbsoluteToLocalPoint(OptionSet<MapCoordinatesMode> mode, TransformState& transformState) const
{
    ASSERT(style().position() == PositionType::Static);

    if (isTransformed())
        mode.remove(IsFixed);

    auto* container = parent();
    if (!container)
        return;

    container->mapAbsoluteToLocalPoint(mode, transformState);

    LayoutSize containerOffset = offsetFromContainer(*container, LayoutPoint());

    pushOntoTransformState(transformState, mode, nullptr, container, containerOffset, false);
}

void RenderSVGModelObject::mapLocalToContainer(const RenderLayerModelObject* ancestorContainer, TransformState& transformState, OptionSet<MapCoordinatesMode> mode, bool* wasFixed) const
{
    mapLocalToSVGContainer(ancestorContainer, transformState, mode, wasFixed);
}

LayoutSize RenderSVGModelObject::offsetFromContainer(RenderElement& container, const LayoutPoint&, bool*) const
{
    ASSERT_UNUSED(container, &container == this->container());
    ASSERT(!isInFlowPositioned());
    ASSERT(!isAbsolutelyPositioned());
    ASSERT(isInline());
    return locationOffsetEquivalent();
}

void RenderSVGModelObject::addFocusRingRects(Vector<LayoutRect>& rects, const LayoutPoint& additionalOffset, const RenderLayerModelObject*) const
{
    auto repaintBoundingBox = enclosingLayoutRect(repaintRectInLocalCoordinates());
    if (repaintBoundingBox.size().isEmpty())
        return;
    rects.append(LayoutRect(additionalOffset, repaintBoundingBox.size()));
}

// FloatRect::intersects does not consider horizontal or vertical lines (because of isEmpty()).
// So special-case handling of such lines.
static bool intersectsAllowingEmpty(const FloatRect& r, const FloatRect& other)
{
    if (r.isEmpty() && other.isEmpty())
        return false;
    if (r.isEmpty() && !other.isEmpty())
        return (other.contains(r.x(), r.y()) && !other.contains(r.maxX(), r.maxY())) || (!other.contains(r.x(), r.y()) && other.contains(r.maxX(), r.maxY()));
    if (other.isEmpty())
        return intersectsAllowingEmpty(other, r);
    return r.intersects(other);
}

// One of the element types that can cause graphics to be drawn onto the target canvas. Specifically: circle, ellipse,
// image, line, path, polygon, polyline, rect, text and use.
static bool isGraphicsElement(const RenderElement& renderer)
{
    return renderer.isSVGShape() || renderer.isSVGText() || renderer.isSVGImage() || renderer.element()->hasTagName(SVGNames::useTag);
}

bool RenderSVGModelObject::checkIntersection(RenderElement* renderer, const FloatRect& rect)
{
    if (!renderer || renderer->style().effectivePointerEvents() == PointerEvents::None)
        return false;
    if (!isGraphicsElement(*renderer))
        return false;
    SVGElement* svgElement = downcast<SVGElement>(renderer->element());
    ASSERT(is<SVGGraphicsElement>(svgElement));
    auto ctm = downcast<SVGGraphicsElement>(*svgElement).getCTM(SVGLocatable::DisallowStyleUpdate);
    return intersectsAllowingEmpty(rect, ctm.mapRect(renderer->repaintRectInLocalCoordinates()));
}

bool RenderSVGModelObject::checkEnclosure(RenderElement* renderer, const FloatRect& rect)
{
    if (!renderer || renderer->style().effectivePointerEvents() == PointerEvents::None)
        return false;
    if (!isGraphicsElement(*renderer))
        return false;
    SVGElement* svgElement = downcast<SVGElement>(renderer->element());
    ASSERT(is<SVGGraphicsElement>(svgElement));
    auto ctm = downcast<SVGGraphicsElement>(*svgElement).getCTM(SVGLocatable::DisallowStyleUpdate);
    return rect.contains(ctm.mapRect(renderer->repaintRectInLocalCoordinates()));
}

LayoutSize RenderSVGModelObject::cachedSizeForOverflowClip() const
{
    ASSERT(hasNonVisibleOverflow());
    ASSERT(hasLayer());
    return layer()->size();
}

bool RenderSVGModelObject::applyCachedClipAndScrollPosition(LayoutRect& rect, const RenderLayerModelObject* container, VisibleRectContext context) const
{
    // Based on RenderBox::applyCachedClipAndScrollPosition -- unused options removed.
    if (!context.options.contains(VisibleRectContextOption::ApplyContainerClip) && this == container)
        return true;

    LayoutRect clipRect(LayoutPoint(), cachedSizeForOverflowClip());
    if (effectiveOverflowX() == Overflow::Visible)
        clipRect.expandToInfiniteX();
    if (effectiveOverflowY() == Overflow::Visible)
        clipRect.expandToInfiniteY();
    bool intersects;
    if (context.options.contains(VisibleRectContextOption::UseEdgeInclusiveIntersection))
        intersects = rect.edgeInclusiveIntersect(clipRect);
    else {
        rect.intersect(clipRect);
        intersects = !rect.isEmpty();
    }
    return intersects;
}

} // namespace WebCore

#endif // ENABLE(LAYER_BASED_SVG_ENGINE)
