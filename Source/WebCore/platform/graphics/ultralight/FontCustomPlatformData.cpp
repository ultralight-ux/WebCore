#include "config.h"
#include "FontCustomPlatformData.h"
#include "FontPlatformData.h"
#include "FontDescription.h"
#include "NotImplemented.h"
#include <wtf/text/WTFString.h>
#include "FreeTypeLib.h"
#include <Ultralight/Buffer.h>

namespace WebCore {

FontCustomPlatformData::FontCustomPlatformData(RefPtr<FT_FaceRec_> face,
  ultralight::RefPtr<ultralight::Buffer> data) : m_face(face), m_data(data)
{
}

FontCustomPlatformData::FontCustomPlatformData(const FontCustomPlatformData& other) {
  m_face = nullptr;
  *this = other;
}

FontCustomPlatformData::~FontCustomPlatformData()
{
  m_face = nullptr;
  m_data = nullptr;
}

FontPlatformData FontCustomPlatformData::fontPlatformData(
  const FontDescription& description, bool bold, bool italic, const FontFeatureSettings&, const FontVariantSettings&, FontSelectionSpecifiedCapabilities)
{
  return FontPlatformData(m_face, m_data, description);
}

FontCustomPlatformData& FontCustomPlatformData::operator=(const FontCustomPlatformData& other) {
  // Check for self-assignment.
  if (this == &other)
    return *this;

  m_data = other.m_data;
  m_face = other.m_face;

  return *this;
}

bool FontCustomPlatformData::supportsFormat(const String& format)
{
  return equalLettersIgnoringASCIICase(format, "truetype")
    || equalLettersIgnoringASCIICase(format, "opentype")
#if USE(WOFF2)
    || equalLettersIgnoringASCIICase(format, "woff2")
#endif
    || equalLettersIgnoringASCIICase(format, "woff");
}

std::unique_ptr<FontCustomPlatformData> createFontCustomPlatformData(
  SharedBuffer& buffer, const String&)
{
  FT_Library freetype = GetFreeTypeLib();
  if (!freetype)
    return nullptr;

  if (buffer.isEmpty())
    return nullptr;

  ultralight::Ref<ultralight::Buffer> fontData = ultralight::Buffer::Create(buffer.data(), buffer.size());

  FT_Face face = nullptr;
  FT_Error error = 0;
  error = FT_New_Memory_Face(freetype,
    (const FT_Byte*)fontData->data(), /* first byte in memory */
    fontData->size(),                 /* size in bytes        */
    0,                                /* face_index           */
    &face                             /* face result          */);
  assert(error == 0);

  return error ? nullptr : std::make_unique<FontCustomPlatformData>(adoptRef(face), fontData);
}

}