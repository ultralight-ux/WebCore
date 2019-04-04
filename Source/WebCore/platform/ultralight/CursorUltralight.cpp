#include "config.h"
#include "Cursor.h"
#include "NotImplemented.h"

namespace WebCore {

Cursor::Cursor(const Cursor& other)
  : m_type(other.m_type)
  , m_image(other.m_image)
  , m_hotSpot(other.m_hotSpot)
#if ENABLE(MOUSE_CURSOR_SCALE)
  , m_imageScaleFactor(other.m_imageScaleFactor)
#endif
  , m_platformCursor(other.m_platformCursor)
{
}

Cursor& Cursor::operator=(const Cursor& other)
{
  m_type = other.m_type;
  m_image = other.m_image;
  m_hotSpot = other.m_hotSpot;
#if ENABLE(MOUSE_CURSOR_SCALE)
  m_imageScaleFactor = other.m_imageScaleFactor;
#endif
  m_platformCursor = other.m_platformCursor;
  return *this;
}

Cursor::~Cursor()
{
}

void Cursor::ensurePlatformCursor() const {
  notImplemented();
}

} // namespace WebCore
