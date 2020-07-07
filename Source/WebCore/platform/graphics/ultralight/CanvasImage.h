#pragma once

#include "FloatSize.h"
#include "Image.h"
#include <wtf/RefPtr.h>
#include <Ultralight/private/Canvas.h>
#include <Ultralight/platform/Surface.h>
#include "PlatformContextUltralight.h"

namespace WebCore {

class CanvasImage final : public Image {
public:
  static RefPtr<CanvasImage> create(const IntSize& size, bool isDeferred)
  {
    return adoptRef(new CanvasImage(size, isDeferred));
  }

  virtual ~CanvasImage();

  ultralight::RefPtr<ultralight::Canvas> canvas() { return m_canvas; }

  // Inherited from Image:
  bool hasSingleSecurityOrigin() const override { return true; }
  void setContainerSize(const FloatSize& size) override {/* do nothing, no resize */ }
  bool usesContainerSize() const override { return true; }
  bool hasRelativeWidth() const override { return true; }
  bool hasRelativeHeight() const override { return true; }
  void computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) override;
  FloatSize size() const override { return FloatSize((float)m_canvas->width(), (float)m_canvas->height()); }
  void destroyDecodedData(bool /*destroyAll*/ = true) override { }

protected:
  ImageDrawResult draw(GraphicsContext&, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator,
    BlendMode, DecodingMode, ImageOrientationDescription) override;
  void drawPattern(GraphicsContext&, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform,
    const FloatPoint& phase, const FloatSize& spacing, CompositeOperator, BlendMode) override;

  bool currentFrameKnownToBeOpaque() const override { return false; }

  CanvasImage(const IntSize& size, bool isDeferred);

private:
  bool isCanvasImage() const override { return true; }

  friend class ImageBuffer;
  ultralight::Surface* m_surface;
  ultralight::RefPtr<ultralight::Canvas> m_canvas;
  std::unique_ptr<GraphicsContext> m_context;
};

}  // namespace WebCore
