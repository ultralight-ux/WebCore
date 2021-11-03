#pragma once

#if USE(ULTRALIGHT)

#if ENABLE(ACCELERATED_2D_CANVAS)
#include "TextureMapper.h"
#if USE(COORDINATED_GRAPHICS_THREADED)
#include "TextureMapperPlatformLayerProxy.h"
#else
#include "TextureMapperPlatformLayer.h"
#endif
#endif

#include "platform/graphics/ultralight/CanvasImage.h"
#include <JavaScriptCore/Uint8ClampedArray.h>

namespace WebCore {

  class IntSize;
  class CanvasImage;

  class ImageBufferData
#if ENABLE(ACCELERATED_2D_CANVAS)
#if USE(COORDINATED_GRAPHICS_THREADED)
    : public TextureMapperPlatformLayerProxyProvider
#else
    : public TextureMapperPlatformLayer
#endif
#endif
  {
  public:
    ImageBufferData(const IntSize&, RenderingMode, bool isDeferred);
    virtual ~ImageBufferData();

    RefPtr<Uint8ClampedArray> getData(AlphaPremultiplication, const IntRect&, const IntSize&) const;
    void putData(const Uint8ClampedArray& source, AlphaPremultiplication sourceFormat, const IntSize& sourceSize, const IntRect& sourceRect, const IntPoint& destPoint, const IntSize&);

    RefPtr<CanvasImage> m_image;
    IntSize m_size;
    RenderingMode m_renderingMode;

#if ENABLE(ACCELERATED_2D_CANVAS)
    void createUltralightGLSurface();

#if USE(COORDINATED_GRAPHICS_THREADED)
    RefPtr<TextureMapperPlatformLayerProxy> proxy() const override { return m_platformLayerProxy.copyRef(); }
    void swapBuffersIfNeeded() override;
    void createCompositorBuffer();

    RefPtr<TextureMapperPlatformLayerProxy> m_platformLayerProxy;
    //RefPtr<ultralight_surface_t> m_compositorSurface;
    uint32_t m_compositorTexture;
    //RefPtr<ultralight_t> m_compositorCr;
#else
    virtual void paintToTextureMapper(TextureMapper&, const FloatRect& target, const TransformationMatrix&, float opacity);
#endif
    uint32_t m_texture;
#endif
  };

} // namespace WebCore

#endif // USE(ULTRALIGHT)
