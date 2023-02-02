/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2010, 2013 Apple Inc. All rights reserved.
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
#include "GraphicsContextCG.h"

#if USE(CG)

#include "AffineTransform.h"
#include "GraphicsContextImpl.h"
#include "GraphicsContextPlatformPrivateCG.h"
#include "Path.h"

#include <pal/spi/cg/CoreGraphicsSPI.h>
#include <wtf/win/GDIObject.h>


namespace WebCore {

static CGContextRef CGContextWithHDC(HDC hdc, bool hasAlpha)
{
    HBITMAP bitmap = static_cast<HBITMAP>(GetCurrentObject(hdc, OBJ_BITMAP));

    DIBPixelData pixelData(bitmap);

    // FIXME: We can get here because we asked for a bitmap that is too big
    // when we have a tiled layer and we're compositing. In that case 
    // bmBitsPixel will be 0. This seems to be benign, so for now we will
    // exit gracefully and look at it later:
    //  https://bugs.webkit.org/show_bug.cgi?id=52041   
    // ASSERT(bitmapBits.bitsPerPixel() == 32);
    if (pixelData.bitsPerPixel() != 32)
        return 0;

    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Little | (hasAlpha ? kCGImageAlphaPremultipliedFirst : kCGImageAlphaNoneSkipFirst);
    CGContextRef context = CGBitmapContextCreate(pixelData.buffer(), pixelData.size().width(), pixelData.size().height(), 8,
                                                 pixelData.bytesPerRow(), sRGBColorSpaceRef(), bitmapInfo);

    // Flip coords
    CGContextTranslateCTM(context, 0, pixelData.size().height());
    CGContextScaleCTM(context, 1, -1);
    
    // Put the HDC In advanced mode so it will honor affine transforms.
    SetGraphicsMode(hdc, GM_ADVANCED);
    
    return context;
}

GraphicsContext::GraphicsContext(HDC hdc, bool hasAlpha)
{
    platformInit(hdc, hasAlpha);
}

void GraphicsContext::platformInit(HDC hdc, bool hasAlpha)
{
    if (!hdc)
        return;

    m_data = new GraphicsContextPlatformPrivate(CGContextWithHDC(hdc, hasAlpha));
    CGContextRelease(m_data->m_cgContext.get());
    m_data->m_hdc = hdc;
    if (m_data->m_cgContext) {
        // Make sure the context starts in sync with our state.
        setPlatformFillColor(fillColor());
        setPlatformStrokeColor(strokeColor());
    }
}

// FIXME: Is it possible to merge getWindowsContext and createWindowsBitmap into a single API
// suitable for all clients?
void GraphicsContext::releaseWindowsContext(HDC hdc, const IntRect& dstRect, bool supportAlphaBlend)
{
    bool createdBitmap = m_impl || !m_data->m_hdc || isInTransparencyLayer();
    if (!createdBitmap) {
        m_data->restore();
        return;
    }

    if (dstRect.isEmpty())
        return;

    auto bitmap = adoptGDIObject(static_cast<HBITMAP>(::GetCurrentObject(hdc, OBJ_BITMAP)));

    DIBPixelData pixelData(bitmap.get());

    ASSERT(pixelData.bitsPerPixel() == 32);

    CGContextRef bitmapContext = CGBitmapContextCreate(pixelData.buffer(), pixelData.size().width(), pixelData.size().height(), 8,
                                                       pixelData.bytesPerRow(), sRGBColorSpaceRef(), kCGBitmapByteOrder32Little |
                                                       (supportAlphaBlend ? kCGImageAlphaPremultipliedFirst : kCGImageAlphaNoneSkipFirst));

    CGImageRef image = CGBitmapContextCreateImage(bitmapContext);
    CGContextDrawImage(m_data->m_cgContext.get(), dstRect, image);
    
    // Delete all our junk.
    CGImageRelease(image);
    CGContextRelease(bitmapContext);
    ::DeleteDC(hdc);
}

void GraphicsContext::drawWindowsBitmap(WindowsBitmap* image, const IntPoint& point)
{
    // FIXME: Creating CFData is non-optimal, but needed to avoid crashing when printing.  Ideally we should 
    // make a custom CGDataProvider that controls the WindowsBitmap lifetime.  see <rdar://6394455>
    RetainPtr<CFDataRef> imageData = adoptCF(CFDataCreate(kCFAllocatorDefault, image->buffer(), image->bufferLength()));
    RetainPtr<CGDataProviderRef> dataProvider = adoptCF(CGDataProviderCreateWithCFData(imageData.get()));
    RetainPtr<CGImageRef> cgImage = adoptCF(CGImageCreate(image->size().width(), image->size().height(), 8, 32, image->bytesPerRow(), sRGBColorSpaceRef(),
                                                         kCGBitmapByteOrder32Little | kCGImageAlphaFirst, dataProvider.get(), 0, true, kCGRenderingIntentDefault));
    CGContextDrawImage(m_data->m_cgContext.get(), CGRectMake(point.x(), point.y(), image->size().width(), image->size().height()), cgImage.get());   
}

void GraphicsContext::drawFocusRing(const Path& path, float width, float offset, const Color& color)
{
    // FIXME: implement
}

// FIXME: This is nearly identical to the GraphicsContext::drawFocusRing function in GraphicsContextMac.mm.
// The code could move to GraphicsContextCG.cpp and be shared.
void GraphicsContext::drawFocusRing(const Vector<FloatRect>& rects, float width, float offset, const Color& color)
{
    if (paintingDisabled())
        return;

    float radius = (width - 1) / 2.0f;
    offset += radius;
    CGColorRef colorRef = color.isValid() ? cachedCGColor(color) : nullptr;

    CGMutablePathRef focusRingPath = CGPathCreateMutable();
    unsigned rectCount = rects.size();
    for (unsigned i = 0; i < rectCount; i++)
        CGPathAddRect(focusRingPath, 0, CGRectInset(rects[i], -offset, -offset));

    CGContextRef context = platformContext();
    CGContextSaveGState(context);

    CGContextBeginPath(context);
    CGContextAddPath(context, focusRingPath);

    // FIXME: We clear the fill color here to avoid getting a black fill when drawing the focus ring.
    // Find out from CG if this is their bug.
    CGContextSetRGBFillColor(context, 0, 0, 0, 0);

    CGContextSetFocusRingWithColor(context, radius, colorRef, 0, (CFDictionaryRef)0);
    CGContextFillPath(context);

    CGPathRelease(focusRingPath);

    CGContextRestoreGState(context);
}

void GraphicsContext::drawDotsForDocumentMarker(const FloatRect& rect, DocumentMarkerLineStyle style)
{
    if (paintingDisabled())
        return;

    if (style.mode != DocumentMarkerLineStyle::Mode::Spelling && style.mode != DocumentMarkerLineStyle::Mode::Grammar)
        return;

    auto point = rect.location();
    auto width = rect.width();
    auto lineThickness = rect.height();
    auto patternGapWidth = lineThickness / 3;
    auto patternWidth = lineThickness + patternGapWidth;

    // Make sure to draw only complete dots.
    // NOTE: Code here used to shift the underline to the left and increase the width
    // to make sure everything gets underlined, but that results in drawing out of
    // bounds (e.g. when at the edge of a view) and could make it appear that the
    // space between adjacent misspelled words was underlined.
    // allow slightly more considering that the pattern ends with a transparent pixel
    float widthMod = fmodf(width, patternWidth);
    if (patternWidth - widthMod > patternGapWidth)
        width -= widthMod;

    // Draw the underline
    CGContextRef context = platformContext();
    CGContextSaveGState(context);

    static constexpr auto spellingPatternColor = Color::red;
    static constexpr auto grammarPatternColor = Color::darkGreen;

    auto [r, g, b, a] = style.mode == DocumentMarkerLineStyle::Mode::Grammar ? grammarPatternColor : spellingPatternColor;
    CGContextSetRGBStrokeColor(context, r, g, b, a);

    CGAffineTransform userToBase = getUserToBaseCTM(context);
    CGPoint phase = CGPointApplyAffineTransform(point, userToBase);

    CGContextSetPatternPhase(context, CGSizeMake(phase.x, phase.y));
    CGContextSetBlendMode(context, kCGBlendModeNormal);
    
    // 3 rows, each offset by half a pixel for blending purposes
    const CGPoint upperPoints[] = {{ point.x(), point.y() + lineThickness - 2.5 }, {point.x() + width, point.y() + lineThickness - 2.5 }};
    const CGPoint middlePoints[] = {{ point.x(), point.y() + lineThickness - 1.5 }, {point.x() + width, point.y() + lineThickness - 1.5 }};
    const CGPoint lowerPoints[] = {{ point.x(), point.y() + lineThickness - 0.5 }, {point.x() + width, point.y() + lineThickness - 0.5 }};
    
    // Dash lengths for the top and bottom of the error underline are the same.
    // These are magic.
    static const CGFloat edge_dash_lengths[] = {2.0f, 2.0f};
    static const CGFloat middle_dash_lengths[] = { 2.76f, 1.24f };
    static const CGFloat edge_offset = -(edge_dash_lengths[1] - 1.0f) / 2.0f;
    static const CGFloat middle_offset = -(middle_dash_lengths[1] - 1.0f) / 2.0f;

    // Line opacities.  Once again, these are magic.
    const float upperOpacity = 0.33f;
    const float middleOpacity = 0.75f;
    const float lowerOpacity = 0.88f;

    //Top line
    CGContextSetLineDash(context, edge_offset, edge_dash_lengths, WTF_ARRAY_LENGTH(edge_dash_lengths));
    CGContextSetAlpha(context, upperOpacity);
    CGContextStrokeLineSegments(context, upperPoints, 2);
 
    // Middle line
    CGContextSetLineDash(context, middle_offset, middle_dash_lengths, WTF_ARRAY_LENGTH(middle_dash_lengths));
    CGContextSetAlpha(context, middleOpacity);
    CGContextStrokeLineSegments(context, middlePoints, 2);
    
    // Bottom line
    CGContextSetLineDash(context, edge_offset, edge_dash_lengths, WTF_ARRAY_LENGTH(edge_dash_lengths));
    CGContextSetAlpha(context, lowerOpacity);
    CGContextStrokeLineSegments(context, lowerPoints, 2);

    CGContextRestoreGState(context);
}

void GraphicsContextPlatformPrivate::flush()
{
    CGContextFlush(m_cgContext.get());
}

}
#endif
