#include "config.h"
#include "TextureMapperUltralight.h"
#include "BitmapTexturePool.h"
#include "FloatQuad.h"
#include "FloatRoundedRect.h"
#include "LengthFunctions.h"
#include "NotImplemented.h"
#include "Path.h"
#include <functional>
#include <memory>

#if USE(TEXTURE_MAPPER_ULTRALIGHT)

#include "BitmapTextureUltralight.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/private/Filter.h>

namespace WebCore {

std::unique_ptr<ultralight::Filter> ToUltralightFilter(RefPtr<FilterOperation> operation, const IntSize& size, int pass, const BitmapTexture* contentTexture, float deviceScale, float adjustScale)
{
    switch (operation->type()) {
    case FilterOperation::Type::Grayscale:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::Grayscale,
            static_cast<const BasicColorMatrixFilterOperation&>(*operation).amount());
    case FilterOperation::Type::Sepia:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::Sepia,
            static_cast<const BasicColorMatrixFilterOperation&>(*operation).amount());
    case FilterOperation::Type::Saturate:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::Saturate,
            static_cast<const BasicColorMatrixFilterOperation&>(*operation).amount());
    case FilterOperation::Type::HueRotate:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::HueRotate,
            static_cast<const BasicColorMatrixFilterOperation&>(*operation).amount());
    case FilterOperation::Type::Invert:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::Invert,
            static_cast<const BasicComponentTransferFilterOperation&>(*operation).amount());
    case FilterOperation::Type::Brightness:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::Brightness,
            static_cast<const BasicComponentTransferFilterOperation&>(*operation).amount());
    case FilterOperation::Type::Contrast:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::Contrast,
            static_cast<const BasicComponentTransferFilterOperation&>(*operation).amount());
    case FilterOperation::Type::Opacity:
        return std::make_unique<ultralight::BasicFilter>(ultralight::Filter::Type::Opacity,
            static_cast<const BasicComponentTransferFilterOperation&>(*operation).amount());
    case FilterOperation::Type::Blur: {
        float blurRadius = TextureMapperUltralight::getBlurRadiusForFilter(operation, size, deviceScale) * adjustScale;
        return std::make_unique<ultralight::BlurFilter>(pass, blurRadius, 0.0f, 0.0f, false);
    }
    case FilterOperation::Type::DropShadow: {
        float blurRadius = TextureMapperUltralight::getBlurRadiusForFilter(operation, size, deviceScale) * adjustScale;

        // Scale the offset by the device scale.
        auto& shadow = static_cast<const DropShadowFilterOperation&>(*operation);
        auto offset = ultralight::Point((float)shadow.location().x(), (float)shadow.location().y());
        offset *= deviceScale;
        offset *= adjustScale;
        offset *= -1.0f; // Invert to convert from shadow offset to sample offset.

        return std::make_unique<ultralight::BlurFilter>(pass, blurRadius, offset.x, offset.y, true);
        /*
        auto& shadow = static_cast<const DropShadowFilterOperation&>(*operation);
        float blurRadius = TextureMapperUltralight::getBlurRadiusForFilter(operation, size, deviceScale) * adjustScale;

        // Scale the offset by the device scale.
        auto offset = ultralight::Point((float)shadow.location().x(), (float)shadow.location().y());
        offset *= deviceScale;
        offset *= adjustScale;

        ultralight::RefPtr<ultralight::Canvas> content;
        if (contentTexture)
            content = static_cast<const BitmapTextureUltralight*>(contentTexture)->canvas();
            
        return std::make_unique<ultralight::DropShadowFilter>(pass, offset, blurRadius, shadow.color(), content);
        */
    }
    default:
        return std::make_unique<ultralight::Filter>();
    }
}


unsigned TextureMapperUltralight::getPassesRequiredForFilter(RefPtr<FilterOperation> filter, bool use_gpu)
{
    switch (filter->type()) {
    case FilterOperation::Type::Grayscale:
    case FilterOperation::Type::Sepia:
    case FilterOperation::Type::Saturate:
    case FilterOperation::Type::HueRotate:
    case FilterOperation::Type::Invert:
    case FilterOperation::Type::Brightness:
    case FilterOperation::Type::Contrast:
    case FilterOperation::Type::Opacity:
        return 1;
    case FilterOperation::Type::Blur:
    case FilterOperation::Type::DropShadow:
        // We use two-passes (vertical+horizontal) for blur and drop-shadow on GPU
        return use_gpu ? 2 : 1;
    default:
        return 0;
    }
}

float TextureMapperUltralight::getBlurRadiusForFilter(RefPtr<FilterOperation> filter, const IntSize& size, float deviceScale)
{
    switch (filter->type()) {
    case FilterOperation::Type::Blur: {
        auto& blur = static_cast<const BlurFilterOperation&>(*filter);

        // Scale the blur amount by the device scale.
        Length blurAmount = blur.stdDeviation();
        blurAmount *= deviceScale;

        // Clamp radius to reasonable limit (max dimension of the texture).
        return floatValueForLength(blurAmount, (float)std::max(size.width(), size.height()));
    }
    case FilterOperation::Type::DropShadow: {
        auto& shadow = static_cast<const DropShadowFilterOperation&>(*filter);

        // Scale the blur amount by the device scale.
        float blurAmount = shadow.stdDeviation() * deviceScale;

        // Clamp radius to reasonable limit (max dimension of the texture).
        return std::min(blurAmount, (float)std::max(size.width(), size.height()));
    }
    default: return 0.0f;
    }

    return 0.0f;
}

float TextureMapperUltralight::getScaleRequiredForFilter(RefPtr<FilterOperation> filter, const IntSize& size, float deviceScale, bool use_gpu)
{
    if (filter->type() != FilterOperation::Type::Blur && filter->type() != FilterOperation::Type::DropShadow)
        return 1.0f;

    float blurRadius = getBlurRadiusForFilter(filter, size, deviceScale);

    // Get the effect quality:
    ultralight::EffectQuality quality = ultralight::Platform::instance().config().effect_quality;

    float downscaleFactor = ultralight::CalculateDownscaleFactorForBlur(blurRadius, size.width(), size.height(), quality, use_gpu);

    return 1.0f / downscaleFactor;
}

TextureMapperUltralight::TextureMapperUltralight(bool use_gpu, double scale)
    : use_gpu_(use_gpu)
    , scale_(scale)
    , paint_id_(0)
    , clip_stack_(IntRect(0, 0, 8192, 8192))
{
    m_texturePool = std::make_unique<BitmapTexturePool>(use_gpu_, this);
}

TextureMapperUltralight::~TextureMapperUltralight() {}

void TextureMapperUltralight::setRootSurface(
    ultralight::RefPtr<ultralight::Canvas> canvas)
{
    if (canvas) {
        if (root_surface_ && static_cast<BitmapTextureUltralight*>(root_surface_.get())->canvas() == canvas)
            return;
        root_surface_ = BitmapTextureUltralight::create(this, canvas);
    }
}

void TextureMapperUltralight::set_bounds(const IntRect& bounds)
{
    if (bounds == bounds_)
        return;

    bounds_ = bounds;

    // Reset the clip stack with the new bounds.
    if (root_surface_)
        root_surface_->clipStack().reset(bounds);
}

void TextureMapperUltralight::drawBorder(const Color& color, float borderThickness,
    const FloatRect& rect, const TransformationMatrix& modelViewMatrix)
{
    if (!current_surface_)
        return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    // bitmapTexture->applyClipIfNeeded(clip_stack_);

    auto canvas = bitmapTexture->canvas();

    // We create a path and stroke it to draw the border:
    auto path = WebCore::Path();
    path.addRect(rect);

    canvas->Save();
    canvas->SetMatrix(modelViewMatrix);

    canvas->StrokePath(path.platformPath(), color, borderThickness);

    canvas->Restore();
}

void TextureMapperUltralight::drawNumber(int number, const Color& color,
    const FloatPoint& position, const TransformationMatrix& matrix) {
    if (!current_surface_)
        return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    auto canvas = bitmapTexture->canvas();

    // Convert the number to a string
    String text = String::number(number);

    // Digit sizing and spacing
    float digitWidth = 14;
    float digitHeight = 18;
    float digitSpacing = 4;
    float padding = 8;
    float strokeWidth = 1.5f;

    // Calculate the total width needed for the string
    float totalWidth = text.length() * digitWidth + (text.length() - 1) * digitSpacing + padding * 2;

    // Draw as a rectangle with the number on it
    FloatRect numberRect(position.x(), position.y(), totalWidth, digitHeight + padding * 2);

    canvas->Save();
    bitmapTexture->applyClip();
    canvas->Transform(matrix);

    // Draw background rectangle 
    canvas->DrawRect(numberRect, UltralightColorBLACK);

    // Draw border
    canvas->DrawRect({ numberRect.x(), numberRect.y(), numberRect.width(), 1 }, color);
    canvas->DrawRect({ numberRect.x(), numberRect.y() + numberRect.height() - 1, numberRect.width(), 1 }, color);
    canvas->DrawRect({ numberRect.x(), numberRect.y(), 1, numberRect.height() }, color);
    canvas->DrawRect({ numberRect.x() + numberRect.width() - 1, numberRect.y(), 1, numberRect.height() }, color);

    // Use white for digit color regardless of the border color (for better visibility)
    Color digitColor = Color::white;

    // Start position for the first digit
    float x = numberRect.x() + padding;
    float y = numberRect.y() + padding;
    float h = digitHeight;

    for (unsigned i = 0; i < text.length(); ++i) {
        char digit = text[i];
        auto path = ultralight::Path::Create();

        switch (digit) {
        case '0':
            path->MoveTo(ultralight::Point(x, y));
            path->LineTo(ultralight::Point(x + digitWidth, y));  // top
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // right
            path->LineTo(ultralight::Point(x, y + h));  // bottom
            path->LineTo(ultralight::Point(x, y));  // left
            break;
        case '1':
            path->MoveTo(ultralight::Point(x + digitWidth / 2, y));
            path->LineTo(ultralight::Point(x + digitWidth / 2, y + h));  // vertical line
            break;
        case '2':
            path->MoveTo(ultralight::Point(x, y));
            path->LineTo(ultralight::Point(x + digitWidth, y));  // top
            path->LineTo(ultralight::Point(x + digitWidth, y + h/2));  // right top
            path->LineTo(ultralight::Point(x, y + h/2));  // middle
            path->LineTo(ultralight::Point(x, y + h));  // left bottom
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // bottom
            break;
        case '3':
            path->MoveTo(ultralight::Point(x, y));
            path->LineTo(ultralight::Point(x + digitWidth, y));  // top
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // right
            path->LineTo(ultralight::Point(x, y + h));  // bottom
            path->MoveTo(ultralight::Point(x, y + h/2));
            path->LineTo(ultralight::Point(x + digitWidth, y + h/2));  // middle
            break;
        case '4':
            path->MoveTo(ultralight::Point(x, y));
            path->LineTo(ultralight::Point(x, y + h/2));  // left top
            path->LineTo(ultralight::Point(x + digitWidth, y + h/2));  // middle
            path->MoveTo(ultralight::Point(x + digitWidth, y));
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // right
            break;
        case '5':
            path->MoveTo(ultralight::Point(x + digitWidth, y));
            path->LineTo(ultralight::Point(x, y));  // top
            path->LineTo(ultralight::Point(x, y + h/2));  // left top
            path->LineTo(ultralight::Point(x + digitWidth, y + h/2));  // middle
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // right bottom
            path->LineTo(ultralight::Point(x, y + h));  // bottom
            break;
        case '6':
            path->MoveTo(ultralight::Point(x + digitWidth, y));
            path->LineTo(ultralight::Point(x, y));  // top
            path->LineTo(ultralight::Point(x, y + h));  // left
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // bottom
            path->LineTo(ultralight::Point(x + digitWidth, y + h/2));  // right bottom
            path->LineTo(ultralight::Point(x, y + h/2));  // middle
            break;
        case '7':
            path->MoveTo(ultralight::Point(x, y));
            path->LineTo(ultralight::Point(x + digitWidth, y));  // top
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // right
            break;
        case '8':
            path->MoveTo(ultralight::Point(x, y));
            path->LineTo(ultralight::Point(x + digitWidth, y));  // top
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // right
            path->LineTo(ultralight::Point(x, y + h));  // bottom
            path->LineTo(ultralight::Point(x, y));  // left
            path->MoveTo(ultralight::Point(x, y + h/2));
            path->LineTo(ultralight::Point(x + digitWidth, y + h/2));  // middle
            break;
        case '9':
            path->MoveTo(ultralight::Point(x, y + h));
            path->LineTo(ultralight::Point(x + digitWidth, y + h));  // bottom
            path->LineTo(ultralight::Point(x + digitWidth, y));  // right
            path->LineTo(ultralight::Point(x, y));  // top
            path->LineTo(ultralight::Point(x, y + h/2));  // left top
            path->LineTo(ultralight::Point(x + digitWidth, y + h/2));  // middle
            break;
        }

        canvas->StrokePath(path, digitColor, strokeWidth);
        x += digitWidth + digitSpacing;
    }

    canvas->Restore();
}

void TextureMapperUltralight::drawTexture(const BitmapTexture& texture,
    const FloatRect& target, const TransformationMatrix& modelViewMatrix,
    float opacity, unsigned exposedEdges)
{
    ProfiledZone;
    if (!texture.isValid())
        return;

    if (!current_surface_)
        return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());

    auto canvas = bitmapTexture->canvas();

    auto& textureUL = static_cast<const BitmapTextureUltralight&>(texture);

    auto srcCanvas = textureUL.canvas();
    
    // Get the actual texture content dimensions
    FloatSize contentSize = texture.contentSize();
    
    // Source rectangle is based on the content size of the texture
    ultralight::Rect src = { 0.0f, 0.0f, (float)contentSize.width(), (float)contentSize.height() };
    
    // Destination rectangle is the target rect provided to this function
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    
    // Apply opacity to the color
    Color color = UltralightRGBA(255, 255, 255, (unsigned char)(opacity * 255.0f));

    std::unique_ptr<ultralight::Filter> filter;
    if (textureUL.filterInfo()->filter) {
        auto ulFilter = ToUltralightFilter(textureUL.filterInfo()->filter, texture.size(), textureUL.filterInfo()->pass, textureUL.filterInfo()->contentTexture.get(), scale(), 1.0f);
        if (ulFilter->type() != ultralight::Filter::Type::None) {
            filter = std::move(ulFilter);
        }
    }

    // Always apply clip stack before drawing
    canvas->Save();

    bitmapTexture->applyClip();
    
    // Apply the transformation matrix
    canvas->Transform(modelViewMatrix);

    if (isInMaskMode())
        canvas->SetCompositeOp(ultralight::kCompositeOp_DestinationIn);

    // Optimize for opaque textures: disable blending when we can do a direct copy
    bool isOpaque = texture.isOpaque() && opacity >= 1.0f && !isInMaskMode() && !filter;
    isOpaque = false;

    // Save current blending state
    bool previousBlendingEnabled = canvas->blending_enabled();

    if (isOpaque) {
        // Disable blending for opaque content (will use SkBlendMode::kSrc for direct copy)
        canvas->set_blending_enabled(false);
    }

    if (filter) {
        canvas->DrawCanvasWithFilter(srcCanvas, filter.get(), src, dest, color);
    } else {
        // Use DrawCanvas method to composite this tile
        canvas->DrawCanvas(srcCanvas, src, dest, color);
    }

    // Restore previous blending state
    if (isOpaque) {
        canvas->set_blending_enabled(previousBlendingEnabled);
    }

    canvas->Restore();

#if 0
    // Visualize clip by drawing a border rect:
    auto clipRect = WebCore::FloatRect(clip_stack_.scissorRect());
    drawBorder(Color::red, 12.0f, clipRect, TransformationMatrix());
#endif
}

void TextureMapperUltralight::drawSolidColor(const FloatRect& target,
    const TransformationMatrix& modelViewMatrix, const Color& color, bool isBlendingAllowed)
{
    ProfiledZone;

    if (!current_surface_)
        return;

    //if (isInMaskMode())
    //    return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    auto canvas = bitmapTexture->canvas();

    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };

    canvas->Save();
    bitmapTexture->applyClip();
    canvas->Transform(modelViewMatrix);
    if (!isBlendingAllowed)
        canvas->set_blending_enabled(false);
    canvas->DrawRect(dest, color);
    if (!isBlendingAllowed)
        canvas->set_blending_enabled(true);
    canvas->Restore();
}

void TextureMapperUltralight::clearColor(const Color&)
{
    // Ultralight TODO
    notImplemented();
}

// makes a surface the target for the following drawTexture calls.
void TextureMapperUltralight::bindSurface(BitmapTexture* surface)
{
    if (!surface) {
        current_surface_ = root_surface_;
        return;
    }

    current_surface_ = static_cast<BitmapTextureUltralight*>(surface);
}

void TextureMapperUltralight::beginClip(const TransformationMatrix& mat, const FloatRoundedRect& rect)
{
    auto* clip = clipStack();
    if (clip)
        clip->pushClip(rect, mat);
}

void TextureMapperUltralight::endClip()
{
    if (clipStack())
        clipStack()->popClip();
}

IntRect TextureMapperUltralight::clipBounds()
{
    //return IntRect(0, 0, 8192, 8192);
    return clipStack() ? clipStack()->scissorRect() : IntRect();
}

Ref<BitmapTexture> TextureMapperUltralight::createTexture()
{
    ProfiledZone;
    return BitmapTextureUltralight::create(this, use_gpu_);
}

Ref<BitmapTexture> TextureMapperUltralight::createTexture(int internalFormat)
{
    return createTexture();
}

void TextureMapperUltralight::setDepthRange(double zNear, double zFar)
{
    // Ultralight TODO
    notImplemented();
}

void TextureMapperUltralight::beginPainting(PaintFlags)
{
    paint_id_++;
    bindSurface(0);
}

void TextureMapperUltralight::endPainting() {}

IntSize TextureMapperUltralight::maxTextureSize() const
{
    // Match native WebKit's threshold for when to start tiling
    return IntSize(2048, 2048);
}

RefPtr<BitmapTexture> TextureMapperUltralight::acquireTextureFromPool(const IntSize& size, const BitmapTexture::Flags flags)
{
    return acquireTextureFromPool(size, flags, false);
}

RefPtr<BitmapTexture> TextureMapperUltralight::acquireTextureFromPool(const IntSize& size, const BitmapTexture::Flags flags, bool needsExactSize)
{
    ProfiledZone;
    // TODO: get opaque tracking working properly with texture mapper textures,
    // for now, force flags to always allow alpha:
    BitmapTexture::Flags alphaFlags = BitmapTexture::SupportsAlpha;
    RefPtr<BitmapTexture> selectedTexture = m_texturePool->acquireTexture(size, alphaFlags, needsExactSize);
    selectedTexture->reset(size, alphaFlags);
    return selectedTexture;
}

void TextureMapperUltralight::drawFiltered(const BitmapTexture& sourceTexture, const IntSize& srcSize, 
    const BitmapTexture* contentTexture, RefPtr<FilterOperation> operation, int pass, float adjustScale, bool blend)
{
    ProfiledZone;
    if (!sourceTexture.isValid())
        return;

    if (!current_surface_)
        return;

    //if (isInMaskMode())
     //   return;

    auto filter = ToUltralightFilter(operation, srcSize, pass, contentTexture, scale(), adjustScale);
    if (filter->type() == ultralight::Filter::Type::None) {
        drawTexture(sourceTexture, FloatRect(0, 0, srcSize.width(), srcSize.height()));
        return;
    }

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    auto canvas = bitmapTexture->canvas();

    FloatRect target(IntPoint::zero(), srcSize);

    auto srcCanvas = static_cast<const BitmapTextureUltralight&>(sourceTexture).canvas();
    ultralight::Rect src = { target.x(), target.y(), target.maxX(), target.maxY() };
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    Color color = UltralightColorWHITE;

    if (operation->type() == FilterOperation::Type::DropShadow) {
        // We apply the drop shadow color to the filter.
        auto& shadow = static_cast<const DropShadowFilterOperation&>(*operation);
        color = shadow.color();
    }

    canvas->Save();
    bool blendingEnabled = canvas->blending_enabled();
    canvas->set_blending_enabled(blend);
    //bitmapTexture->applyClipIfNeeded(clip_stack_);
    canvas->DrawCanvasWithFilter(srcCanvas, filter.get(), src, dest, color);
    canvas->set_blending_enabled(blendingEnabled);
    canvas->Restore();

#if 0
    // Visualize clip by drawing a border rect:
    auto clipRect = WebCore::FloatRect(clip_stack_.scissorRect());
    drawBorder(Color::black, 12.0f, clipRect, TransformationMatrix());
#endif
}

void TextureMapperUltralight::drawTextureWithScale(const BitmapTexture& sourceTexture, const IntSize& srcSize, const IntSize& dstSize, const FloatPoint& dstOffset, bool blend)
{
    ProfiledZone;
    if (!sourceTexture.isValid())
        return;

    if (!current_surface_)
        return;

    //if (isInMaskMode())
     //   return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    auto canvas = bitmapTexture->canvas();

    auto srcCanvas = static_cast<const BitmapTextureUltralight&>(sourceTexture).canvas();
    ultralight::Rect src = { 0.0f, 0.0f, (float)srcSize.width(), (float)srcSize.height() };
    ultralight::Rect dest = { 0.0f, 0.0f, (float)dstSize.width(), (float)dstSize.height() };
    dest.Move(dstOffset.x(), dstOffset.y());
    Color color = UltralightColorWHITE;

    canvas->Save();
    bool blendingEnabled = canvas->blending_enabled();
    canvas->set_blending_enabled(blend);
    //bitmapTexture->applyClipIfNeeded(clip_stack_);
    canvas->DrawCanvas(srcCanvas, src, dest, color);
    canvas->set_blending_enabled(blendingEnabled);
    canvas->Restore();

#if 0
    // Visualize clip by drawing a border rect:
    auto clipRect = WebCore::FloatRect(clip_stack_.scissorRect());
    drawBorder(Color::red, 12.0f, clipRect, TransformationMatrix());
#endif
}

} // namespace WebCore

#endif
