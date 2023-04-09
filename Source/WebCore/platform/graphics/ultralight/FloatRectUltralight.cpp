#include "config.h"
#include "FloatRect.h"
#include "FloatRoundedRect.h"

#if USE(ULTRALIGHT)

#include <Ultralight/Geometry.h>

namespace WebCore {

FloatRect::FloatRect(const ultralight::Rect& r)
  : m_location(r.x(), r.y())
  , m_size(r.width(), r.height())
{
}

FloatRect::operator ultralight::Rect() const
{
  ultralight::Rect r = { x(), y(), x() + width(), y() + height() };
  return r;
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
