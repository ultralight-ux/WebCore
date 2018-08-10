#include "config.h"
#include "FontCache.h"
#include "Font.h"
#include "NotImplemented.h"
#include "FreeTypeLib.h"
#include <Ultralight/Platform/FontLoader.h>
#include <Ultralight/Platform/Platform.h>
#include <Ultralight/private/Painter.h>
#include "StringUltralight.h"

namespace WebCore {

using ultralight::Convert;

RefPtr<Font> FontCache::systemFallbackForCharacters(const FontDescription& description, const Font* originalFontData,
  bool isPlatformFont, const UChar* characters, unsigned length)
{
  // TODO
  return lastResortFallbackFont(description);
}

Vector<String> FontCache::systemFontFamilies()
{
  // TODO
  notImplemented();
  return Vector<String>();
}

void FontCache::platformInit()
{
  GetFreeTypeLib();
}

/*
Vector<FontSelectionCapabilities> FontCache::getFontSelectionCapabilitiesInFamily(const AtomicString&) {
  // TODO
  notImplemented();
  return Vector<FontSelectionCapabilities>();
}
*/

Ref<Font> FontCache::lastResortFallbackFontForEveryCharacter(const FontDescription& fontDescription)
{
  return lastResortFallbackFont(fontDescription);
}

Ref<Font> FontCache::lastResortFallbackFont(const FontDescription& fontDescription)
{
  auto& platform = ultralight::Platform::instance();
  auto font_loader = platform.font_loader();
  return *fontForFamily(fontDescription, Convert(font_loader->fallback_font()));
}

Vector<FontTraitsMask> FontCache::getTraitsInFamily(const AtomicString&)
{
  return{};
}

int GetRawWeight(FontWeight weight) {
  switch (weight) {
  case FontWeight100: return 100;
  case FontWeight200: return 200;
  case FontWeight300: return 300;
  case FontWeight400: return 400;
  case FontWeight500: return 500;
  case FontWeight600: return 600;
  case FontWeight700: return 700;
  case FontWeight800: return 800;
  case FontWeight900: return 900;
  default: return 300;
  }
}

std::unique_ptr<FontPlatformData> FontCache::createFontPlatformData(const FontDescription& fontDescription,
  const AtomicString& family, const FontFeatureSettings*, const FontVariantSettings*)
{
  platformInit();

  int error = 0;
  FT_Library freetype = GetFreeTypeLib();
  if (!freetype)
    return nullptr;

  auto& platform = ultralight::Platform::instance();
  auto loader = platform.font_loader();
  if (!loader)
    return nullptr;

  auto font_family = family;
  if (font_family.isEmpty())
    font_family = Convert(loader->fallback_font());

  auto font = loader->Load(Convert(font_family.string()), GetRawWeight(fontDescription.weight()),
    fontDescription.italic() == FontItalicOn, fontDescription.computedSize());
  
  size_t font_size = font->size();

  if (font_size) {
    FT_Face face;
    FT_Error error;
    error = FT_New_Memory_Face(freetype,
      (const FT_Byte*)font->data(), /* first byte in memory */
                      font->size(), /* size in bytes        */
                      0,            /* face_index           */
                      &face         /* face result          */);
    assert(error == 0);
    if (error != 0)
      return nullptr;

    error = FT_Set_Pixel_Sizes(face, 0, fontDescription.computedPixelSize());
    assert(error == 0);
    return std::make_unique<FontPlatformData>(face, font, fontDescription);
  }

  return nullptr;
}

const AtomicString& FontCache::platformAlternateFamilyName(const AtomicString&) {
  // TODO
  notImplemented();
  static AtomicString empty;

  return empty;
}

}  // namespace WebCore
