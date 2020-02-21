#include "config.h"
#include "FontCache.h"
#include "Font.h"
#include "NotImplemented.h"
#include "FreeTypeLib.h"
#include <wtf/text/CString.h>
#include <Ultralight/platform/FontLoader.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/Painter.h>
#include "StringUltralight.h"

namespace WebCore {

using ultralight::Convert;

static int GetRawWeight(FontSelectionValue weight) {
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

RefPtr<Font> FontCache::systemFallbackForCharacters(const FontDescription& description, 
  const Font* originalFontData, IsForPlatformFont, PreferColoredFont preferColoredFont, 
  const UChar* characters, unsigned length)
{
  auto& platform = ultralight::Platform::instance();
  auto loader = platform.font_loader();
  if (!loader)
    return lastResortFallbackFont(description);

  auto fallback = loader->fallback_font_for_characters(
    ultralight::String16(reinterpret_cast<const ultralight::Char16*>(characters), length),
    GetRawWeight(description.weight()), !!description.italic(), description.computedSize());

  return fontForFamily(description, Convert(fallback));
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


Vector<FontSelectionCapabilities> FontCache::getFontSelectionCapabilitiesInFamily(const AtomString&, AllowUserInstalledFonts) {
  // TODO
  notImplemented();
  return Vector<FontSelectionCapabilities>();
}

bool FontCache::isSystemFontForbiddenForEditing(WTF::String const &) {
	notImplemented();
	return false;
}


/*
Ref<Font> FontCache::lastResortFallbackFontForEveryCharacter(const FontDescription& fontDescription)
{
  return lastResortFallbackFont(fontDescription);
}
*/

Ref<Font> FontCache::lastResortFallbackFont(const FontDescription& fontDescription)
{
  auto& platform = ultralight::Platform::instance();
  auto font_loader = platform.font_loader();
  return *fontForFamily(fontDescription, Convert(font_loader->fallback_font()));
}

/*
Vector<FontTraitsMask> FontCache::getTraitsInFamily(const AtomicString&)
{
  return{};
}
*/

std::unique_ptr<FontPlatformData> FontCache::createFontPlatformData(const FontDescription& fontDescription,
  const AtomString& family, const FontFeatureSettings*, const FontVariantSettings*, FontSelectionSpecifiedCapabilities)
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
    !!fontDescription.italic(), fontDescription.computedSize());
  
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

const AtomString& FontCache::platformAlternateFamilyName(const AtomString&) {
  // TODO
  return nullAtom();
}

}  // namespace WebCore
