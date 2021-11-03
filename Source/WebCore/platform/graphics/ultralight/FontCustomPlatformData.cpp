#include "config.h"
#include "FontCustomPlatformData.h"
#include "FontPlatformData.h"
#include "FontDescription.h"
#include "NotImplemented.h"
#include <wtf/text/WTFString.h>
#include "FreeTypeLib.h"
#include <Ultralight/Buffer.h>

namespace WebCore {

FontCustomPlatformData::FontCustomPlatformData(ultralight::RefPtr<ultralight::FontFace> face) : m_face(face)
{
}

FontCustomPlatformData::FontCustomPlatformData(const FontCustomPlatformData& other) {
  m_face = nullptr;
  *this = other;
}

FontCustomPlatformData::~FontCustomPlatformData()
{
  m_face = nullptr;
}

FontPlatformData FontCustomPlatformData::fontPlatformData(
  const FontDescription& description, bool bold, bool italic, const FontFeatureSettings&, const FontVariantSettings&, FontSelectionSpecifiedCapabilities)
{
  return FontPlatformData(m_face, description);
}

FontCustomPlatformData& FontCustomPlatformData::operator=(const FontCustomPlatformData& other) {
  // Check for self-assignment.
  if (this == &other)
    return *this;

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

  ultralight::RefPtr<ultralight::Buffer> fontData = ultralight::Buffer::Create(buffer.data(), buffer.size());

  ultralight::RefPtr<ultralight::FontFile> fontFile = ultralight::FontFile::Create(fontData);

  FT_Face face = nullptr;
  FT_Error error = 0;
  error = FT_New_Memory_Face(freetype,
    (const FT_Byte*)fontData->data(), /* first byte in memory */
    fontData->size(),                 /* size in bytes        */
    0,                                /* face_index           */
    &face                             /* face result          */);
  assert(error == 0);

  if (error)
    return nullptr;

  ultralight::RefPtr<ultralight::FontFace> fontFace = ultralight::FontFace::Create(adoptRef(face), fontFile);

  return std::make_unique<FontCustomPlatformData>(fontFace);
}

}