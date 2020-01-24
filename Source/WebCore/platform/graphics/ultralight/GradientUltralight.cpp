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
        grad->num_stops = 0;
        grad->p0 = ultralight::Point(data.point0.x(), data.point0.y());
        grad->p1 = ultralight::Point(data.point1.x(), data.point1.y());
        return grad;
      },
      [&](const RadialData& data) -> PlatformGradient {
        auto grad = new ultralight::Gradient();
        grad->is_radial = true;
        grad->num_stops = 0;
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

    size_t num_stops = m_stops.size();

    // Clamp to 12 stops
    if (num_stops > 12)
      num_stops = 12;

    m_gradient->num_stops = num_stops;
    for (size_t i = 0; i < num_stops; ++i) {
      m_gradient->stops[i].stop = m_stops[i].offset;
      m_gradient->stops[i].color = UltralightRGBA(m_stops[i].color.red(), m_stops[i].color.green(), m_stops[i].color.blue(), m_stops[i].color.alpha());
    }

    return m_gradient;
  }

  void Gradient::fill(GraphicsContext& context, const FloatRect& rect)
  {
    context.platformContext()->canvas()->DrawGradient(platformGradient(), rect);
  }

} // namespace WebCore
