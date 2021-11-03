#pragma once

#include "config.h"
#include "ImageBuffer.h"

#if USE(ULTRALIGHT)

#include "BitmapImage.h"
#include "GraphicsContext.h"
#include "NotImplemented.h"
#include "platform/graphics/ultralight/CanvasImage.h"
#include <Ultralight/private/Canvas.h>

using namespace std;

namespace WebCore {

ImageBufferData::ImageBufferData(const IntSize& size, RenderingMode renderingMode, bool isDeferred)
    : m_size(size)
    , m_image(CanvasImage::create(size, isDeferred))
{
}

ImageBufferData::~ImageBufferData()
{
}

RefPtr<Uint8ClampedArray> ImageBufferData::getData(AlphaPremultiplication outputFormat, const IntRect& rect, const IntSize& size) const
{
    Checked<unsigned, RecordOverflow> area = 4;
    area *= rect.width();
    area *= rect.height();
    if (area.hasOverflowed())
        return nullptr;

    if (area.unsafeGet() == 0)
        return nullptr;

    auto result = Uint8ClampedArray::tryCreateUninitialized(area.unsafeGet());
    uint8_t* resultData = result ? result->data() : nullptr;
    if (!resultData)
        return nullptr;

    Checked<int> endx = rect.maxX();
    Checked<int> endy = rect.maxY();
    if (rect.x() < 0 || rect.y() < 0 || endx.unsafeGet() > size.width() || endy.unsafeGet() > size.height())
        result->zeroFill();

    int originx = rect.x();
    int destx = 0;
    Checked<int> destw = rect.width();
    if (originx < 0) {
        destw += originx;
        destx = -originx;
        originx = 0;
    }
    destw = std::min<int>(destw.unsafeGet(), size.width() - originx);
    if (endx.unsafeGet() > size.width())
        endx = size.width();
    Checked<int> width = endx - originx;

    int originy = rect.y();
    int desty = 0;
    Checked<int> desth = rect.height();
    if (originy < 0) {
        desth += originy;
        desty = -originy;
        originy = 0;
    }
    desth = std::min<int>(desth.unsafeGet(), size.height() - originy);
    if (endy.unsafeGet() > size.height())
        endy = size.height();
    Checked<int> height = endy - originy;

    if (width.unsafeGet() <= 0 || height.unsafeGet() <= 0)
        return result;

    unsigned destBytesPerRow = 4 * rect.width();
    uint8_t* destRows = resultData + desty * destBytesPerRow + destx * 4;

    unsigned srcBytesPerRow;
    uint8_t* srcRows;

    auto canvas = m_image->canvas();
    canvas->FlushSurface();
    auto surface = canvas->surface();
    if (!surface)
        return result;

    void* data = surface->LockPixels();
    if (!data)
        return result;

    srcBytesPerRow = surface->row_bytes();
    srcRows = reinterpret_cast<uint8_t*>(data) + originy * srcBytesPerRow + originx * 4;

    if (outputFormat == AlphaPremultiplication::Unpremultiplied) {
        if ((width * 4).hasOverflowed())
            CRASH();
        for (int y = 0; y < height.unsafeGet(); ++y) {
            for (int x = 0; x < width.unsafeGet(); x++) {
                int basex = x * 4;
                uint8_t alpha = srcRows[basex + 3];
                if (alpha) {
                    destRows[basex] = (srcRows[basex] * 255) / alpha;
                    destRows[basex + 1] = (srcRows[basex + 1] * 255) / alpha;
                    destRows[basex + 2] = (srcRows[basex + 2] * 255) / alpha;
                    destRows[basex + 3] = alpha;
                } else
                    reinterpret_cast<uint32_t*>(destRows + basex)[0] = reinterpret_cast<const uint32_t*>(srcRows + basex)[0];
            }
            srcRows += srcBytesPerRow;
            destRows += destBytesPerRow;
        }
    } else {
        for (int y = 0; y < height.unsafeGet(); ++y) {
            for (int x = 0; x < (width * 4).unsafeGet(); x += 4)
                reinterpret_cast<uint32_t*>(destRows + x)[0] = reinterpret_cast<uint32_t*>(srcRows + x)[0];
            srcRows += srcBytesPerRow;
            destRows += destBytesPerRow;
        }
    }

    surface->UnlockPixels();

    return result;
}

void ImageBufferData::putData(const Uint8ClampedArray& source, AlphaPremultiplication sourceFormat, const IntSize& sourceSize, const IntRect& sourceRect, const IntPoint& destPoint, const IntSize& size)
{
#if ASSERT_DISABLED
    UNUSED_PARAM(size);
#endif

    ASSERT(sourceRect.width() > 0);
    ASSERT(sourceRect.height() > 0);

    Checked<int> originx = sourceRect.x();
    Checked<int> destx = (Checked<int>(destPoint.x()) + sourceRect.x());
    ASSERT(destx.unsafeGet() >= 0);
    ASSERT(destx.unsafeGet() < size.width());
    ASSERT(originx.unsafeGet() >= 0);
    ASSERT(originx.unsafeGet() <= sourceRect.maxX());

    Checked<int> endx = (Checked<int>(destPoint.x()) + sourceRect.maxX());
    ASSERT(endx.unsafeGet() <= size.width());

    Checked<int> width = sourceRect.width();
    Checked<int> destw = endx - destx;

    Checked<int> originy = sourceRect.y();
    Checked<int> desty = (Checked<int>(destPoint.y()) + sourceRect.y());
    ASSERT(desty.unsafeGet() >= 0);
    ASSERT(desty.unsafeGet() < size.height());
    ASSERT(originy.unsafeGet() >= 0);
    ASSERT(originy.unsafeGet() <= sourceRect.maxY());

    Checked<int> endy = (Checked<int>(destPoint.y()) + sourceRect.maxY());
    ASSERT(endy.unsafeGet() <= size.height());

    Checked<int> height = sourceRect.height();
    Checked<int> desth = endy - desty;

    if (width <= 0 || height <= 0)
        return;

    unsigned srcBytesPerRow = 4 * sourceSize.width();
    const uint8_t* srcRows = source.data() + (originy * srcBytesPerRow + originx * 4).unsafeGet();
    unsigned destBytesPerRow;
    uint8_t* destRows;

    auto canvas = m_image->canvas();
    canvas->FlushSurface();
    auto surface = canvas->surface();
    if (!surface)
        return;

    void* data = surface->LockPixels();
    if (!data)
        return;

    destBytesPerRow = surface->row_bytes();
    destRows = reinterpret_cast<uint8_t*>(data) + (desty * destBytesPerRow + destx * 4).unsafeGet();

    for (int y = 0; y < height.unsafeGet(); ++y) {
        for (int x = 0; x < width.unsafeGet(); x++) {
            int basex = x * 4;
            uint8_t alpha = srcRows[basex + 3];
            if (sourceFormat == AlphaPremultiplication::Unpremultiplied && alpha != 255) {
                destRows[basex] = (srcRows[basex] * alpha + 254) / 255;
                destRows[basex + 1] = (srcRows[basex + 1] * alpha + 254) / 255;
                destRows[basex + 2] = (srcRows[basex + 2] * alpha + 254) / 255;
                destRows[basex + 3] = alpha;
            } else
                reinterpret_cast<uint32_t*>(destRows + basex)[0] = reinterpret_cast<const uint32_t*>(srcRows + basex)[0];
        }
        destRows += destBytesPerRow;
        srcRows += srcBytesPerRow;
    }

    surface->UnlockPixels();
    
    // Update dirty bounds
    ultralight::IntRect dirtyRect { destx.unsafeGet(), desty.unsafeGet(), (destx + width).unsafeGet(), (desty + height).unsafeGet() };
    dirtyRect.Join(surface->dirty_bounds());
    surface->set_dirty_bounds(dirtyRect);
}

ImageBuffer::ImageBuffer(const FloatSize& size, float resolutionScale, ColorSpace, RenderingMode renderingMode, const HostWindow*, bool isDeferred, bool& success)
    : m_data(IntSize(size), renderingMode, isDeferred)
    , m_size(size)
    , m_logicalSize(size)
    , m_resolutionScale(resolutionScale)
{
    success = false; // Make early return mean error.
    if (m_size.isEmpty())
        return;

    success = true;
}

ImageBuffer::~ImageBuffer()
{
}

GraphicsContext& ImageBuffer::context() const
{
    return *m_data.m_image->m_context;
}

RefPtr<Image> ImageBuffer::sinkIntoImage(std::unique_ptr<ImageBuffer> imageBuffer, PreserveResolution preserve)
{
    return imageBuffer->copyImage(DontCopyBackingStore, preserve);
}

RefPtr<Image> ImageBuffer::copyImage(BackingStoreCopy copyBehavior, PreserveResolution) const
{
    // TODO
    //notImplemented();
    //return RefPtr<Image>();
    return m_data.m_image;
}

BackingStoreCopy ImageBuffer::fastCopyImageMode()
{
    return DontCopyBackingStore;
}

void ImageBuffer::drawConsuming(std::unique_ptr<ImageBuffer> imageBuffer, GraphicsContext& destContext, const FloatRect& destRect, const FloatRect& srcRect, CompositeOperator op, BlendMode blendMode)
{
    imageBuffer->draw(destContext, destRect, srcRect, op, blendMode);
}

void ImageBuffer::draw(GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect,
    CompositeOperator op, BlendMode blendMode)
{
    m_data.m_image->draw(context, destRect, srcRect, op, blendMode, DecodingMode::Auto, ImageOrientationDescription());
}

void ImageBuffer::drawPattern(GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform,
    const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blend)
{
    m_data.m_image->drawPattern(context, destRect, srcRect, patternTransform, phase, spacing, op, blend);
}

void ImageBuffer::platformTransformColorSpace(const std::array<uint8_t, 256>&)
{
    // TODO
    notImplemented();
}

RefPtr<Uint8ClampedArray> ImageBuffer::getUnmultipliedImageData(const IntRect& rect, IntSize* pixelArrayDimensions, CoordinateSystem coordinateSystem) const
{
    IntRect srcRect = rect;
    if (coordinateSystem == LogicalCoordinateSystem)
        srcRect.scale(m_resolutionScale);

    if (pixelArrayDimensions)
        *pixelArrayDimensions = srcRect.size();

    return m_data.getData(AlphaPremultiplication::Unpremultiplied, srcRect, internalSize());
}

RefPtr<Uint8ClampedArray> ImageBuffer::getPremultipliedImageData(const IntRect& rect, IntSize* pixelArrayDimensions, CoordinateSystem coordinateSystem) const
{
    IntRect srcRect = rect;
    if (coordinateSystem == LogicalCoordinateSystem)
        srcRect.scale(m_resolutionScale);

    if (pixelArrayDimensions)
        *pixelArrayDimensions = srcRect.size();

    return m_data.getData(AlphaPremultiplication::Premultiplied, srcRect, internalSize());
}

void ImageBuffer::putByteArray(const Uint8ClampedArray& source, AlphaPremultiplication sourceFormat, const IntSize& sourceSize, const IntRect& sourceRect, const IntPoint& destPoint, CoordinateSystem coordinateSystem)
{
    IntRect scaledSourceRect = sourceRect;
    IntSize scaledSourceSize = sourceSize;
    if (coordinateSystem == LogicalCoordinateSystem) {
        scaledSourceRect.scale(m_resolutionScale);
        scaledSourceSize.scale(m_resolutionScale);
    }

    m_data.putData(source, sourceFormat, scaledSourceSize, scaledSourceRect, destPoint, internalSize());
}

String ImageBuffer::toDataURL(const String& mimeType, Optional<double> quality, PreserveResolution) const
{
    // TODO
    notImplemented();

    return String();
}

Vector<uint8_t> ImageBuffer::toData(const String& mimeType, Optional<double> quality) const
{
    // TODO
    notImplemented();

    return Vector<uint8_t>();
}

Vector<uint8_t> ImageBuffer::toBGRAData() const
{
    // TODO
    notImplemented();

    return Vector<uint8_t>();
}

#if ENABLE(ACCELERATED_2D_CANVAS) && !USE(COORDINATED_GRAPHICS_THREADED)
void ImageBufferData::paintToTextureMapper(TextureMapper& textureMapper, const FloatRect& targetRect, const TransformationMatrix& matrix, float opacity)
{
    notImplemented();
}
#endif

} // namespace WebCore

#endif // USE(ULTRALIGHT)
