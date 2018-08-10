#include "config.h"
#include "Image.h"
#include "NotImplemented.h"
#include "GraphicsContext.h"
#include "ImageObserver.h"

namespace WebCore {

  void Image::drawPattern(GraphicsContext& ctx, const FloatRect& destRect,
    const FloatRect& srcRect, const AffineTransform& patternTransform, const FloatPoint& phase,
    const FloatSize& spacing, CompositeOperator op,BlendMode blendMode) {
    ctx.drawPattern(*this, destRect, srcRect, patternTransform, phase, spacing, op, blendMode);

    if (imageObserver())
      imageObserver()->didDraw(this);
  }
} // namespace WebCore
