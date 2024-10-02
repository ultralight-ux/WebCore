#include "config.h"
#include "FloatRect.h"
#include "FloatRoundedRect.h"

#if USE(ULTRALIGHT)

#include <Ultralight/Geometry.h>
#include <functional>

namespace WebCore {

FloatRoundedRect::FloatRoundedRect(const ultralight::RoundedRect& r)
    : m_rect(r.rect)
    , m_radii(FloatSize(r.radii_x[0], r.radii_y[0]),
              FloatSize(r.radii_x[1], r.radii_y[1]),
              FloatSize(r.radii_x[3], r.radii_y[3]),
              FloatSize(r.radii_x[2], r.radii_y[2]))
{
}

FloatRoundedRect::operator ultralight::RoundedRect() const
{
    ultralight::RoundedRect rrect;
    rrect.rect = m_rect;
    rrect.radii_x[0] = m_radii.topLeft().width();
    rrect.radii_x[1] = m_radii.topRight().width();
    rrect.radii_x[2] = m_radii.bottomRight().width();
    rrect.radii_x[3] = m_radii.bottomLeft().width();
    rrect.radii_y[0] = m_radii.topLeft().height();
    rrect.radii_y[1] = m_radii.topRight().height();
    rrect.radii_y[2] = m_radii.bottomRight().height();
    rrect.radii_y[3] = m_radii.bottomLeft().height();
    return rrect;
}

size_t FloatRoundedRect::hash() const
{
    // Compute the combined hash of m_rect and m_radii:
    size_t hashValue = std::hash<float>{}(m_rect.x());
    hashValue = hashValue * 31 + std::hash<float>{}(m_rect.y());
    hashValue = hashValue * 31 + std::hash<float>{}(m_rect.width());
    hashValue = hashValue * 31 + std::hash<float>{}(m_rect.height());
    
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.topLeft().width());
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.topLeft().height());
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.topRight().width());
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.topRight().height());
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.bottomLeft().width());
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.bottomLeft().height());
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.bottomRight().width());
    hashValue = hashValue * 31 + std::hash<float>{}(m_radii.bottomRight().height());
    
    return hashValue;
}
} // namespace WebCore

#endif // USE(ULTRALIGHT)
