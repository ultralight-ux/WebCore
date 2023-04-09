#include "config.h"
#include "Gradient.h"

#include "GraphicsContext.h"
#include "NotImplemented.h"
#include <Ultralight/private/Canvas.h>
#include <Ultralight/private/Paint.h>
#include <algorithm>

namespace WebCore {

void Gradient::stopsChanged()
{
    m_gradient.reset();
}

void Gradient::createUltralightGradient()
{
    m_stops.sort();

    m_gradient = WTF::switchOn(
        m_data,
        [&](const LinearData& data) -> PlatformGradient {
            auto grad = std::make_unique<ultralight::Gradient>();
            grad->is_radial = false;
            grad->num_stops = 0;
            grad->p0 = ultralight::Point(data.point0.x(), data.point0.y());
            grad->p1 = ultralight::Point(data.point1.x(), data.point1.y());
            return grad;
        },
        [&](const RadialData& data) -> PlatformGradient {
            auto grad = std::make_unique<ultralight::Gradient>();
            grad->is_radial = true;
            grad->num_stops = 0;
            grad->p0 = ultralight::Point(data.point0.x(), data.point0.y());
            grad->p1 = ultralight::Point(data.point1.x(), data.point1.y());
            grad->r0 = data.startRadius;
            grad->r1 = data.endRadius;
            return grad;
        },
        [&](const ConicData&) -> PlatformGradient {
            // FIXME: implement conic gradient rendering.
            return nullptr;
        });

    // Bail here if we are conic. (TODO)
    if (!m_gradient)
        return;

    FloatPoint mapped_p0 = FloatPoint(m_gradient->p0.x, m_gradient->p0.y);
    FloatPoint mapped_p1 = FloatPoint(m_gradient->p1.x, m_gradient->p1.y);
    m_gradient->p0 = ultralight::Point(mapped_p0.x(), mapped_p0.y());
    m_gradient->p1 = ultralight::Point(mapped_p1.x(), mapped_p1.y());

    size_t num_stops = m_stops.size();

    // Clamp to 12 stops
    if (num_stops > 12)
        num_stops = 12;

    m_gradient->num_stops = num_stops;
    for (size_t i = 0; i < num_stops; ++i) {
        m_gradient->stops[i].stop = m_stops.stops()[i].offset;
        m_gradient->stops[i].color = m_stops.stops()[i].color;
    }
}

void Gradient::fill(GraphicsContext& context, const FloatRect& rect)
{
    if (!m_gradient)
        createUltralightGradient();

    if (m_gradient)
        context.platformContext()->DrawGradient(m_gradient.get(), rect);
}

} // namespace WebCore
