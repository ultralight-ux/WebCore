#include "config.h"
#include "TextureMapperUltralight.h"
#include "BitmapTexturePool.h"
#include "FloatQuad.h"
#include "FloatRoundedRect.h"
#include "LengthFunctions.h"
#include "NotImplemented.h"
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
        auto& blur = static_cast<const BlurFilterOperation&>(*operation);

        // Clamp radius to reasonable limit (max dimension of the texture).
        float blurRadius = TextureMapperUltralight::getBlurRadiusForFilter(operation, size, deviceScale) * adjustScale;
        return std::make_unique<ultralight::BlurFilter>(pass, blurRadius);
    }
    case FilterOperation::Type::DropShadow: {
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
    , clip_stack_(IntRect(0, 0, 8192, 8192))
{
    m_texturePool = std::make_unique<BitmapTexturePool>(use_gpu_);
}

TextureMapperUltralight::~TextureMapperUltralight() {}

void TextureMapperUltralight::set_default_surface(
    ultralight::RefPtr<ultralight::Canvas> canvas)
{
    default_surface_ = BitmapTextureUltralight::create(canvas);
}

void TextureMapperUltralight::drawBorder(const Color& color, float borderThickness,
    const FloatRect& rect, const TransformationMatrix& modelViewMatrix)
{
    if (!current_surface_)
        return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    // bitmapTexture->applyClipIfNeeded(clip_stack_);

    auto canvas = bitmapTexture->canvas();

    // We do a fill of four rects to simulate the stroke of a border.
    FloatRect rects[4] = {
        FloatRect(rect.x(), rect.y(), rect.width(), borderThickness),
        FloatRect(rect.x(), rect.maxY() - borderThickness, rect.width(), borderThickness),
        FloatRect(rect.x(), rect.y() + borderThickness, borderThickness, rect.height() - 2 * borderThickness),
        FloatRect(rect.maxX() - borderThickness, rect.y() + borderThickness, borderThickness, rect.height() - 2 * borderThickness)
    };

    canvas->Save();
    canvas->Transform(modelViewMatrix);

    for (size_t i = 0; i < 4; ++i)
        canvas->DrawRect(rects[i], color);

    canvas->Restore();
}

void TextureMapperUltralight::drawNumber(int number, const Color&,
    const FloatPoint&, const TransformationMatrix&) {}

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
    ultralight::Rect src = { 0.0f, 0.0f, (float)srcCanvas->width(), (float)srcCanvas->height() };
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    Color color = UltralightRGBA(255, 255, 255, (unsigned char)(opacity * 255.0f));

    std::unique_ptr<ultralight::Filter> filter;
    if (textureUL.filterInfo()->filter) {
        auto ulFilter = ToUltralightFilter(textureUL.filterInfo()->filter, texture.size(), textureUL.filterInfo()->pass, textureUL.filterInfo()->contentTexture.get(), scale(), 1.0f);
        if (ulFilter->type() != ultralight::Filter::Type::None) {
            filter = std::move(ulFilter);
        }
    }

    if (filter) {
        canvas->Save();
        bitmapTexture->applyClipIfNeeded(clip_stack_);
        canvas->Transform(modelViewMatrix);
        canvas->DrawCanvasWithFilter(srcCanvas, filter.get(), src, dest, color);
        canvas->Restore();
    } else {
        canvas->Save();
        canvas->Transform(modelViewMatrix);
        canvas->DrawCanvas(srcCanvas, src, dest, color);
        canvas->Restore();
    }

    // paint.color = UltralightColorGREEN;
    // current_surface_->DrawRect({ 20, 20, 30, 30 }, paint);
}

void TextureMapperUltralight::drawSolidColor(const FloatRect& target,
    const TransformationMatrix& modelViewMatrix, const Color& color, bool isBlendingAllowed)
{
    ProfiledZone;

    if (!current_surface_)
        return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    // bitmapTexture->applyClipIfNeeded(clip_stack_);

    auto canvas = bitmapTexture->canvas();

    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };

    canvas->Save();
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
        current_surface_ = default_surface_;
        return;
    }

    current_surface_ = surface;
}

void TextureMapperUltralight::beginClip(const TransformationMatrix& mat, const FloatRoundedRect& rect)
{
    clip_stack_.pushClip(rect, mat);

    /*
    ProfiledZone;
    auto surface = current_surface_ ? current_surface_ : default_surface_;
    auto canvas = static_cast<BitmapTextureUltralight*>(surface.get())->canvas();
    canvas->Save();

    // TODO: support 3d transforms in clip
    if (!mat.isAffine())
        return;

    FloatQuad quad = mat.projectQuad(rect.rect());
    IntRect bbox = quad.enclosingBoundingBox();

    // Only use scissors on rectilinear clips.
    if (!quad.isRectilinear() || bbox.isEmpty())
        return;

    ultralight::IntRect scissor_box = { bbox.x(), bbox.y(), bbox.maxX(), bbox.maxY() };
    ultralight::IntRect new_scissor = canvas->GetScissorRect().Intersect(scissor_box);

    canvas->set_scissor_enabled(true);
    canvas->SetScissorRect(new_scissor);
    */
}

void TextureMapperUltralight::endClip()
{
    clip_stack_.popClip();

    /*
    ProfiledZone;
    auto surface = current_surface_ ? current_surface_ : default_surface_;
    auto canvas = static_cast<BitmapTextureUltralight*>(surface.get())->canvas();
    canvas->Restore();
    */
}

IntRect TextureMapperUltralight::clipBounds()
{
    return clip_stack_.scissorRect();

    /*
    auto surface = current_surface_ ? current_surface_ : default_surface_;
    auto canvas = static_cast<BitmapTextureUltralight*>(surface.get())->canvas();
    ultralight::IntRect bounds = canvas->GetScissorRect();
    return IntRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
    */
}

Ref<BitmapTexture> TextureMapperUltralight::createTexture()
{
    ProfiledZone;
    BitmapTextureUltralight* texture = new BitmapTextureUltralight(use_gpu_);
    return *adoptRef(texture);
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
    bindSurface(0);
}

void TextureMapperUltralight::endPainting() {}

IntSize TextureMapperUltralight::maxTextureSize() const
{
    return IntSize(4096, 4096);
}

void TextureMapperUltralight::drawFiltered(const BitmapTexture& sourceTexture,
    const BitmapTexture* contentTexture, RefPtr<FilterOperation> operation, int pass, float adjustScale)
{
    ProfiledZone;
    if (!sourceTexture.isValid())
        return;

    if (!current_surface_)
        return;


    auto filter = ToUltralightFilter(operation, sourceTexture.size(), pass, contentTexture, scale(), adjustScale);
    if (filter->type() == ultralight::Filter::Type::None) {
        drawTexture(sourceTexture, FloatRect(0, 0, sourceTexture.size().width(), sourceTexture.size().height()));
        return;
    }

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    // bitmapTexture->applyClipIfNeeded(clip_stack_);

    auto canvas = bitmapTexture->canvas();

    FloatRect target(IntPoint::zero(), sourceTexture.contentSize());

    auto srcCanvas = static_cast<const BitmapTextureUltralight&>(sourceTexture).canvas();
    ultralight::Rect src = { 0.0f, 0.0f, (float)srcCanvas->width(), (float)srcCanvas->height() };
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    Color color = UltralightColorWHITE;

    canvas->Save();
    canvas->DrawCanvasWithFilter(srcCanvas, filter.get(), src, dest, color);
    canvas->Restore();
}

void TextureMapperUltralight::drawTextureWithScale(const BitmapTexture& sourceTexture, const IntSize& srcSize, const IntSize& dstSize)
{
    ProfiledZone;
    if (!sourceTexture.isValid())
        return;

    if (!current_surface_)
        return;

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    // bitmapTexture->applyClipIfNeeded(clip_stack_);

    auto canvas = bitmapTexture->canvas();

    FloatRect target(IntPoint::zero(), sourceTexture.contentSize());

    auto srcCanvas = static_cast<const BitmapTextureUltralight&>(sourceTexture).canvas();
    ultralight::Rect src = { 0.0f, 0.0f, (float)srcSize.width(), (float)srcSize.height() };
    ultralight::Rect dest = { 0.0f, 0.0f, (float)dstSize.width(), (float)dstSize.height() };
    Color color = UltralightColorWHITE;

    canvas->Save();
    canvas->DrawCanvas(srcCanvas, src, dest, color);
    canvas->Restore();
}

} // namespace WebCore

#endif
