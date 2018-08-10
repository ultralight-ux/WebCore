#include "config.h"
#include "HarfBuzzFace.h"

#include "Font.h"
#include "FontPlatformData.h"
#include "GlyphBuffer.h"
#include "HarfBuzzShaper.h"
#include "TextEncoding.h"
#include <hb.h>
#include <hb-ft.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringView.h>

namespace WebCore {

hb_face_t* HarfBuzzFace::createFace()
{
  return hb_ft_face_create(m_platformData->face(), NULL);
}

hb_font_t* HarfBuzzFace::createFont()
{
  return hb_ft_font_create(m_platformData->face(), NULL);
}

GlyphBufferAdvance HarfBuzzShaper::createGlyphBufferAdvance(float width, float height)
{
  return GlyphBufferAdvance(width, height);
}

}  // namespace WebCore
