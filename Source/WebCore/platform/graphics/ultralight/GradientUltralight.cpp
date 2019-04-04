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

    m_gradient = new ultralight::Gradient();
    m_gradient->p0 = ultralight::Point(m_p0.x(), m_p0.y());
    m_gradient->p1 = ultralight::Point(m_p1.x(), m_p1.y());
    m_gradient->r0 = m_r0;
    m_gradient->r1 = m_r1;
    m_gradient->is_radial = m_radial;
    
    size_t num_stops = m_stops.size();

    // Clamp to 12 stops
    if (num_stops > 12)
      num_stops = 12;

    m_gradient->num_stops = num_stops;
    for (size_t i = 0; i < num_stops; ++i) {
      m_gradient->stops[i].stop = m_stops[i].stop;
      m_gradient->stops[i].color[0] = m_stops[i].red;
      m_gradient->stops[i].color[1] = m_stops[i].green;
      m_gradient->stops[i].color[2] = m_stops[i].blue;
      m_gradient->stops[i].color[3] = m_stops[i].alpha;
    }

    return m_gradient;
  }

  void Gradient::fill(GraphicsContext* context, const FloatRect& rect)
  {
    context->platformContext()->canvas()->DrawGradient(platformGradient(), rect);
  }

} // namespace WebCore
