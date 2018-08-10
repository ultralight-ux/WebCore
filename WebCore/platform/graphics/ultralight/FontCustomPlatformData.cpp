#include "config.h"
#include "FontCustomPlatformData.h"
#include "FontPlatformData.h"
#include "FontDescription.h"
#include "NotImplemented.h"
#include <wtf/text/WTFString.h>
#include "FreeTypeLib.h"
#include <Ultralight/Buffer.h>

namespace WebCore {

FontCustomPlatformData::FontCustomPlatformData(FT_Face face, 
  ultralight::RefPtr<ultralight::Buffer> data) : m_face(face), m_data(data)
{
}

FontCustomPlatformData::~FontCustomPlatformData()
{
  FT_Done_Face(m_face);
  m_data = nullptr;
}

FontPlatformData FontCustomPlatformData::fontPlatformData(
  const FontDescription& description, bool bold, bool italic)
{
  // Increase the ref-count before passing it to FontPlatformData
  FT_Reference_Face(m_face);
  return FontPlatformData(m_face, m_data, description);
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
  SharedBuffer& buffer)
{
  FT_Library freetype = GetFreeTypeLib();
  if (!freetype)
    return nullptr;

  if (buffer.isEmpty())
    return nullptr;

  ultralight::Ref<ultralight::Buffer> fontData = ultralight::Buffer::Create(buffer.data(), buffer.size());

  FT_Face face;
  FT_Error error;
  error = FT_New_Memory_Face(freetype,
    (const FT_Byte*)fontData->data(), /* first byte in memory */
    fontData->size(),                 /* size in bytes        */
    0,                                /* face_index           */
    &face                             /* face result          */);
  assert(error == 0);

  return error ? nullptr : std::make_unique<FontCustomPlatformData>(face, fontData);
}

}