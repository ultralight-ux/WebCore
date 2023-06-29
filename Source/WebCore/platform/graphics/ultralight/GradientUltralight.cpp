#include "config.h"
#include "Gradient.h"

#include "GraphicsContext.h"
#include "PlatformContextUltralight.h"
#include "NotImplemented.h"
#include <algorithm>
#include <Ultralight/private/Paint.h>
#include <Ultralight/private/Canvas.h>

namespace WebCore {

  void Gradient::platformDestroy()
  {
    if (m_gradient)
      delete m_gradient;
  }

  PlatformGradient Gradient::platformGradient()
  {
    if (m_gradient)
      return m_gradient;

    m_gradient = WTF::switchOn(m_data,
      [&](const LinearData& data) -> PlatformGradient {
        auto grad = new ultralight::Gradient();
        grad->is_radial = false;
        grad->p0 = ultralight::Point(data.point0.x(), data.point0.y());
        grad->p1 = ultralight::Point(data.point1.x(), data.point1.y());
        return grad;
      },
      [&](const RadialData& data) -> PlatformGradient {
        auto grad = new ultralight::Gradient();
        grad->is_radial = true;
        grad->p0 = ultralight::Point(data.point0.x(), data.point0.y());
        grad->p1 = ultralight::Point(data.point1.x(), data.point1.y());
        grad->r0 = data.startRadius;
        grad->r1 = data.endRadius;
        return grad;
      },
        [&](const ConicData&)  -> PlatformGradient {
        // FIXME: implement conic gradient rendering.
        return nullptr;
      }
    );

    // Bail here if we are conic. (TODO)
    if (!m_gradient)
      return m_gradient;

    // TODO: Apply this transformation in shader instead of applying it to p0/p1
    FloatPoint mapped_p0 = m_gradientSpaceTransformation.mapPoint(FloatPoint(m_gradient->p0.x, m_gradient->p0.y));
    FloatPoint mapped_p1 = m_gradientSpaceTransformation.mapPoint(FloatPoint(m_gradient->p1.x, m_gradient->p1.y));
    m_gradient->p0 = ultralight::Point(mapped_p0.x(), mapped_p0.y());
    m_gradient->p1 = ultralight::Point(mapped_p1.x(), mapped_p1.y());

    size_t num_stops = m_stops.size();

    // Reserve capacity ahead of time to reduce reallocations
    m_gradient->stops.reserve(num_stops);

    for (size_t i = 0; i < num_stops; ++i) {
      ultralight::GradientStop stop;
      stop.stop = m_stops[i].offset;
      stop.color = UltralightRGBA(m_stops[i].color.red(), m_stops[i].color.green(), m_stops[i].color.blue(), m_stops[i].color.alpha());
      m_gradient->stops.push_back(stop);
    }

    return m_gradient;
  }

  void Gradient::fill(GraphicsContext& context, const FloatRect& rect)
  {
    context.platformContext()->canvas()->DrawGradient(platformGradient(), rect);
  }

} // namespace WebCore
