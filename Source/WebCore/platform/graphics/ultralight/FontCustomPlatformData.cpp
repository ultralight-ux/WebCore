#include "config.h"
#include "FontCustomPlatformData.h"
#include "FontPlatformData.h"
#include "FontDescription.h"
#include "NotImplemented.h"
#include <wtf/text/WTFString.h>
#include "FreeTypeLib.h"
#include "FreeTypeFace.h"
#include "StringUltralight.h"
#include <Ultralight/Buffer.h>

namespace WebCore {

inline int ToRawWeight(FontSelectionValue weight) {
  if (weight < FontSelectionValue(150)) return 100;
  if (weight < FontSelectionValue(250)) return 200;
  if (weight < FontSelectionValue(350)) return 300;
  if (weight < FontSelectionValue(450)) return 400;
  if (weight < FontSelectionValue(550)) return 500;
  if (weight < FontSelectionValue(650)) return 600;
  if (weight < FontSelectionValue(750)) return 700;
  if (weight < FontSelectionValue(850)) return 800;
  return 900;
}

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
  const FontDescription& description, bool bold, bool italic, const FontCreationContext& fontCreationContext)
{
  int font_weight = ToRawWeight(description.weight());
  bool font_italic = !!description.italic();

  auto font_file = m_face->font_file();
  if (!font_file)
    return FontPlatformData();

  auto buffer = font_file->buffer();
  if (!buffer)
    return FontPlatformData();

  auto own_face = FreeTypeFace::createFromBuffer(buffer);
  if (!own_face)
    return FontPlatformData();

  return FontPlatformData(m_face, own_face, description, font_weight, font_italic);
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
  return equalLettersIgnoringASCIICase(format, "truetype"_s)
      || equalLettersIgnoringASCIICase(format, "opentype"_s)
#if USE(WOFF2)
      || equalLettersIgnoringASCIICase(format, "woff2"_s)
#endif
      || equalLettersIgnoringASCIICase(format, "woff"_s);
}

std::unique_ptr<FontCustomPlatformData> createFontCustomPlatformData(
  SharedBuffer& buffer, const String&)
{
  ProfiledMemoryZone(MemoryTag::Font);

  if (buffer.isEmpty())
    return nullptr;

  ultralight::RefPtr<ultralight::Buffer> fontData = ultralight::Buffer::CreateFromCopy(buffer.data(), buffer.size());
  ultralight::RefPtr<ultralight::FontFile> fontFile = ultralight::FontFile::Create(fontData);

  RefPtr<FreeTypeFace> ft_face = FreeTypeFace::createFromBuffer(fontData);
  if (!ft_face)
    return nullptr;

  ultralight::RefPtr<ultralight::FontFace> fontFace = ultralight::FontFace::Create(ft_face, fontFile);

  return std::make_unique<FontCustomPlatformData>(fontFace);
}

}
