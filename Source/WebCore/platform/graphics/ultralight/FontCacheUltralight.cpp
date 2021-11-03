#include "config.h"
#include "FontCache.h"
#include "Font.h"
#include "NotImplemented.h"
#include "FreeTypeLib.h"
#include "PlatformFontFreeType.h"
#include <wtf/text/CString.h>
#include <Ultralight/platform/FontLoader.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/Painter.h>
#include <Ultralight/platform/Logger.h>
#include <Ultralight/private/util/Debug.h>
#include <Ultralight/private/util/RefCountedImpl.h>
#include <Ultralight/private/tracy/Tracy.hpp>
#include <wtf/text/StringHasher.h>
#include "StringUltralight.h"
#include <map>

namespace ultralight {

class FontFaceImpl : public FontFace,
                     public RefCountedImpl<FontFaceImpl> {
public:
  virtual void AddRef() const override { return RefCountedImpl<FontFaceImpl>::AddRef(); }
  virtual void Release() const override { return RefCountedImpl<FontFaceImpl>::Release(); }
  virtual int ref_count() const override { return RefCountedImpl<FontFaceImpl>::ref_count(); }

  virtual WTF::RefPtr<FT_FaceRec_> face() const override { return face_; }

  virtual RefPtr<FontFile> font_file() const { return font_file_; }

  virtual void update_access() override {
    last_access_ = std::chrono::steady_clock::now();
  }

  virtual std::chrono::steady_clock::time_point last_access() const { return last_access_; }

protected:
  FontFaceImpl(WTF::RefPtr<FT_FaceRec_> face, RefPtr<FontFile> font_file) : face_(face), font_file_(font_file) {
    last_access_ = std::chrono::steady_clock::now();
  }

  ~FontFaceImpl() {}

  WTF::RefPtr<FT_FaceRec_> face_;
  RefPtr<FontFile> font_file_;
  std::chrono::steady_clock::time_point last_access_;

  friend class FontFace;
  friend class RefCountedImpl<FontFaceImpl>;
};

FontFace::FontFace() {}
FontFace::~FontFace() {}

RefPtr<FontFace> FontFace::Create(WTF::RefPtr<FT_FaceRec_> face, RefPtr<FontFile> font_file) {
  return AdoptRef(*new FontFaceImpl(face, font_file));
}

class FontDatabase {
public:
  static FontDatabase& instance() {
    static FontDatabase g_instance;
    return g_instance;
  }

  RefPtr<FontFace> LookupFont(const String& family, int weight, bool italic) {
    ProfiledZone;
    ultralight::String8 family8 = family.utf8();
    unsigned int family_hash = StringHasher::hashMemory(family8.data(), family8.sizeBytes());
    uintptr_t hashCodes[] = { family_hash, (uintptr_t)weight, italic };
    unsigned int request_hash = StringHasher::hashMemory<sizeof(hashCodes)>(hashCodes);

    auto i = font_db_.find(request_hash);
    if (i != font_db_.end()) {
      if (i->second)
        i->second->update_access();
      return i->second;
    }

    // Doesn't exist in database, need to load and create new
    auto& platform = ultralight::Platform::instance();
    auto font_loader = platform.font_loader();
    UL_CHECK(font_loader, "Error, NULL FontLoader encountered, did you forget to call Platform::set_font_loader()?");
    if (!font_loader)
      return nullptr;

    FT_Library freetype = WebCore::GetFreeTypeLib();
    if (!freetype)
      return nullptr;

    RefPtr<FontFile> file = font_loader->Load(family, weight, italic);
    RefPtr<FontFace> font_face;
    if (file) {
      if (file->is_in_memory()) {
        ultralight::RefPtr<ultralight::Buffer> font_file_buffer = file->buffer();
        if (!font_file_buffer)
          return nullptr;

        size_t font_size = font_file_buffer->size();

        if (font_size) {
          FT_Face face;
          FT_Error error;
          error = FT_New_Memory_Face(freetype,
            (const FT_Byte*)font_file_buffer->data(), /* first byte in memory */
            font_file_buffer->size(),                 /* size in bytes        */
            0,                                        /* face_index           */
            &face                                     /* face result          */);
          assert(error == 0);
          if (error != 0)
            return nullptr;

          assert(error == 0);
          font_face = FontFace::Create(adoptRef(face), file);
        }
      }
      else {
        ultralight::String font_file_path = file->filepath();
        if (font_file_path.empty())
          return nullptr;

        FT_Face face;
        FT_Error error;
        CString path8 = Convert(font_file_path).utf8();
        error = FT_New_Face(freetype, path8.data(), 0, &face);
        assert(error == 0);
        if (error != 0)
          return nullptr;

        assert(error == 0);
        font_face = FontFace::Create(adoptRef(face), file);
      }
    }
    
    // font_face may still be null here, that's okay-- we want to cache a request that resulted in a null load
    font_db_.insert({ request_hash, font_face });
    if (font_face)
      font_face->update_access();

    return font_face;
  }

  void Recycle() {
    ProfiledZone;
    for (auto i = font_db_.begin(); i != font_db_.end();) {
      // Evict the entry if we are the only ones holding the reference
      if (i->second && i->second->ref_count() <= 1) {
        // Only remove if entry hasn't been accessed in a while.
        auto last_access = i->second->last_access();
        auto now = std::chrono::steady_clock::now();
        long long age_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_access).count();
        if (age_ms > entry_keep_alive_ms_) {
          i = font_db_.erase(i);
          continue;
        }
      }
      
      i++;
    }
  }

protected:
  FontDatabase() {}
  ~FontDatabase() {}

  typedef std::map<unsigned int, RefPtr<FontFace>> FontFaceMap;
  FontFaceMap font_db_;

  // How long to keep an entry alive after it has been accessed and has no lingering references.
  // Defaults to 7 seconds
  const long long entry_keep_alive_ms_ = 7000;
};

}  // namespace ultralight

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
  ProfiledZone;
  auto& platform = ultralight::Platform::instance();
  auto font_loader = platform.font_loader();
  UL_CHECK(font_loader, "Error, NULL FontLoader encountered, did you forget to call Platform::set_font_loader()?");
  
  if (!font_loader)
    return nullptr;

  auto fallback = font_loader->fallback_font_for_characters(
    ultralight::String16(reinterpret_cast<const ultralight::Char16*>(characters), length),
    GetRawWeight(description.weight()), !!description.italic());

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
  ultralight::EnsurePlatformFontFactory();
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
  UL_CHECK(font_loader, "Error, NULL FontLoader encountered, did you forget to call Platform::set_font_loader()?");
  WTF::String fallback;
  if (font_loader)
    fallback = Convert(font_loader->fallback_font());
  return *fontForFamily(fontDescription, fallback);
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
  ProfiledZone;
  platformInit();

  int error = 0;
  FT_Library freetype = GetFreeTypeLib();
  if (!freetype)
    return nullptr;

  auto& platform = ultralight::Platform::instance();
  auto font_loader = platform.font_loader();
  UL_CHECK(font_loader, "Error, NULL FontLoader encountered, did you forget to call Platform::set_font_loader()?");
  if (!font_loader)
    return nullptr;

  auto font_family = family;
  if (font_family.isEmpty())
    font_family = Convert(font_loader->fallback_font());

  ultralight::FontDatabase& font_db = ultralight::FontDatabase::instance();

  ultralight::RefPtr<ultralight::FontFace> font_face = font_db.LookupFont(
    Convert(font_family.string()), GetRawWeight(fontDescription.weight()), !!fontDescription.italic());

  if (!font_face)
    return nullptr;

  return std::make_unique<FontPlatformData>(font_face, fontDescription);;
}

const AtomString& FontCache::platformAlternateFamilyName(const AtomString&) {
  // TODO
  return nullAtom();
}

void FontCache::platformPurgeInactiveFontData()
{
  ultralight::FontDatabase::instance().Recycle();
}

}  // namespace WebCore
