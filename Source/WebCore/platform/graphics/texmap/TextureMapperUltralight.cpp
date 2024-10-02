#include "config.h"
#include "TextureMapperUltralight.h"
#include "BitmapTexturePool.h"
#include "FloatQuad.h"
#include "FloatRoundedRect.h"
#include "LengthFunctions.h"
#include "NotImplemented.h"
#include <functional>

#if USE(TEXTURE_MAPPER_ULTRALIGHT)

#include "BitmapTextureUltralight.h"
#include <Ultralight/private/Filter.h>

namespace WebCore {

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
    bitmapTexture->applyClipIfNeeded(clip_stack_);
    
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
    bitmapTexture->applyClipIfNeeded(clip_stack_);
    
    auto canvas = bitmapTexture->canvas();

    auto srcCanvas = static_cast<const BitmapTextureUltralight&>(texture).canvas();
    ultralight::Rect src = { 0.0f, 0.0f, (float)srcCanvas->width(), (float)srcCanvas->height() };
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    Color color = UltralightRGBA(255, 255, 255, (unsigned char)(opacity * 255.0f));

    canvas->Save();
    canvas->Transform(modelViewMatrix);
    canvas->DrawCanvas(srcCanvas, src, dest, color);
    canvas->Restore();

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
    bitmapTexture->applyClipIfNeeded(clip_stack_);
    
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

ultralight::Filter ToUltralightFilter(const FilterOperation& operation, const IntSize& size, int pass, const BitmapTexture* contentTexture)
{
    switch(operation.type()) {
    case FilterOperation::Type::Grayscale:
        return ultralight::BasicFilter(ultralight::Filter::Type::Grayscale,
            static_cast<const BasicColorMatrixFilterOperation&>(operation).amount());
    case FilterOperation::Type::Sepia:
        return ultralight::BasicFilter(ultralight::Filter::Type::Sepia,
            static_cast<const BasicColorMatrixFilterOperation&>(operation).amount());
    case FilterOperation::Type::Saturate:
        return ultralight::BasicFilter(ultralight::Filter::Type::Saturate,
            static_cast<const BasicColorMatrixFilterOperation&>(operation).amount());
    case FilterOperation::Type::HueRotate:
        return ultralight::BasicFilter(ultralight::Filter::Type::HueRotate,
            static_cast<const BasicColorMatrixFilterOperation&>(operation).amount());
    case FilterOperation::Type::Invert:
        return ultralight::BasicFilter(ultralight::Filter::Type::Invert,
            static_cast<const BasicComponentTransferFilterOperation&>(operation).amount());
    case FilterOperation::Type::Brightness:
        return ultralight::BasicFilter(ultralight::Filter::Type::Brightness,
            static_cast<const BasicComponentTransferFilterOperation&>(operation).amount());
    case FilterOperation::Type::Contrast:
        return ultralight::BasicFilter(ultralight::Filter::Type::Contrast,
            static_cast<const BasicComponentTransferFilterOperation&>(operation).amount());
    case FilterOperation::Type::Opacity:
        return ultralight::BasicFilter(ultralight::Filter::Type::Opacity,
            static_cast<const BasicComponentTransferFilterOperation&>(operation).amount());
    case FilterOperation::Type::Blur: {
        auto& blur = static_cast<const BlurFilterOperation&>(operation);
        FloatSize radius;
        radius.setHeight(floatValueForLength(blur.stdDeviation(), size.height()) / size.height());
        radius.setWidth(floatValueForLength(blur.stdDeviation(), size.width()) / size.width());
        return ultralight::BlurFilter(pass, radius.width(), radius.height());
    }
    case FilterOperation::Type::DropShadow: {
        auto& shadow = static_cast<const DropShadowFilterOperation&>(operation);
        ultralight::RefPtr<ultralight::Canvas> content;
        if (contentTexture)
            content = static_cast<const BitmapTextureUltralight*>(contentTexture)->canvas();
        return ultralight::DropShadowFilter(
            pass, ultralight::Point(shadow.location().x(), shadow.location().y()),
            shadow.stdDeviation() / float(size.width()),
            shadow.stdDeviation() / float(size.height()),
            shadow.color(), content);
    }
    default:
        return ultralight::Filter();
    }
}

void TextureMapperUltralight::drawFiltered(const BitmapTexture& sourceTexture,
    const BitmapTexture* contentTexture, const FilterOperation& operation, int pass)
{
    ProfiledZone;
    if (!sourceTexture.isValid())
        return;

    if (!current_surface_)
        return;

    ultralight::Filter filter = ToUltralightFilter(operation, sourceTexture.size(), pass, contentTexture);
    if (filter.type() == ultralight::Filter::Type::None) {
        drawTexture(sourceTexture, FloatRect(0, 0, sourceTexture.size().width(), sourceTexture.size().height()));
        return;
    }

    auto bitmapTexture = static_cast<BitmapTextureUltralight*>(current_surface_.get());
    bitmapTexture->applyClipIfNeeded(clip_stack_);
    
    auto canvas = bitmapTexture->canvas();

    FloatRect target(IntPoint::zero(), sourceTexture.contentSize());

    auto srcCanvas = static_cast<const BitmapTextureUltralight&>(sourceTexture).canvas();
    ultralight::Rect src = { 0.0f, 0.0f, (float)srcCanvas->width(), (float)srcCanvas->height() };
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    Color color = UltralightColorWHITE;

    canvas->Save();
    canvas->DrawCanvasWithFilter(srcCanvas, filter, src, dest, color);
    canvas->Restore();
}

} // namespace WebCore

#endif
