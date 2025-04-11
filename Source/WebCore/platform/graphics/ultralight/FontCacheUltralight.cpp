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
#include <wtf/Shutdown.h>
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

  virtual WTF::Vector<FT_Fixed> design_coordinates() const { return design_coordinates_; }

  virtual void update_access() override {
    last_access_ = std::chrono::steady_clock::now();
  }

  virtual std::chrono::steady_clock::time_point last_access() const { return last_access_; }

protected:
  FontFaceImpl(WTF::RefPtr<FT_FaceRec_> face, RefPtr<FontFile> font_file, WTF::Vector<FT_Fixed> design_coordinates) : face_(face), font_file_(font_file), design_coordinates_(design_coordinates) {
    last_access_ = std::chrono::steady_clock::now();
  }

  ~FontFaceImpl() {}

  WTF::RefPtr<FT_FaceRec_> face_;
  RefPtr<FontFile> font_file_;
  WTF::Vector<FT_Fixed> design_coordinates_;
  std::chrono::steady_clock::time_point last_access_;

  friend class FontFace;
  friend class RefCountedImpl<FontFaceImpl>;
};

FontFace::FontFace() {}
FontFace::~FontFace() {}

RefPtr<FontFace> FontFace::Create(WTF::RefPtr<FT_FaceRec_> face, RefPtr<FontFile> font_file, WTF::Vector<FT_Fixed> design_coordinates) {
  return AdoptRef(*new FontFaceImpl(face, font_file, design_coordinates));
}

class FontDatabase {
public:
  static FontDatabase& instance() {
    static FontDatabase* g_instance = nullptr;

    if (!g_instance)
    {
        g_instance = new FontDatabase();
        WTF::CallOnShutdown([]() mutable {
            delete g_instance;
            g_instance = nullptr;
        });
    }

    return *g_instance;
  }

  WTF::Vector<FT_Fixed> CalculateDesignCoordinates(FT_Face ft_face, int weight, bool italic) {
    WTF::Vector<FT_Fixed> coords;
    FT_MM_Var* mmvar = nullptr;
    FT_Error error = FT_Get_MM_Var(ft_face, &mmvar);
    
    if (error == 0 && mmvar) {
      coords.resize(mmvar->num_axis);
      
      // Initialize all coordinates to their default values
      for (FT_UInt j = 0; j < mmvar->num_axis; j++) {
        coords[j] = mmvar->axis[j].def;
      }
      
      // Track whether we found weight and italic axes
      bool found_weight = false;
      bool found_italic = false;
      
      // Find the weight and italic/slant axes
      for (FT_UInt i = 0; i < mmvar->num_axis; i++) {
        FT_Var_Axis& axis = mmvar->axis[i];
        FT_ULong tag = axis.tag;
        
        // Check if this is the weight axis (tag 'wght')
        if (tag == FT_MAKE_TAG('w', 'g', 'h', 't')) {
          // Map CSS weight (100-900) to the font's weight axis range
          double normalized = (weight - 100) / 800.0; // Normalize between 0 and 1
          coords[i] = axis.minimum + 
                      (FT_Fixed)(normalized * (axis.maximum - axis.minimum));
          found_weight = true;
        }
        // Check if this is the italic axis (tag 'ital')
        else if (tag == FT_MAKE_TAG('i', 't', 'a', 'l')) {
          // Set italic coordinate based on the italic boolean
          coords[i] = italic ? axis.maximum : axis.minimum;
          found_italic = true;
        }
        // Check if this is the slant axis (tag 'slnt') - an alternative to 'ital'
        else if (tag == FT_MAKE_TAG('s', 'l', 'n', 't') && !found_italic) {
          // Set slant coordinate based on the italic boolean
          // Note: slant axis is often negative for italic (e.g. -12 degrees)
          coords[i] = italic ? axis.minimum : axis.def;
          found_italic = true;
        }
      }
      
      // Free the MM_Var structure
      FT_Done_MM_Var(ft_face->glyph->library, mmvar);
    }

    return coords;
  }

  RefPtr<FontFace> LookupFont(const String& family, int weight, bool italic) {
    ProfiledZone;
    ProfiledMemoryZone(MemoryTag::Font);
    ultralight::String8 family8 = family.utf8();
    unsigned int family_hash = StringHasher::computeHash(family8.data(), (unsigned int)family8.sizeBytes());
    unsigned int request_hash = family_hash;
    request_hash = request_hash * 31 + weight;
    request_hash = request_hash * 31 + (italic ? 1 : 0);


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

          // Calculate design coordinates to apply weight/italic to variable fonts
          WTF::Vector<FT_Fixed> design_coordinates = CalculateDesignCoordinates(face, weight, italic);

          font_face = FontFace::Create(adoptRef(face), file, design_coordinates);
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

        // Calculate design coordinates to apply weight/italic to variable fonts
        WTF::Vector<FT_Fixed> design_coordinates = CalculateDesignCoordinates(face, weight, italic);

        font_face = FontFace::Create(adoptRef(face), file, design_coordinates);
      }
    }
    
    // font_face may still be null here, that's okay-- we want to cache a request that resulted in a null load
    font_db_.insert({ request_hash, font_face });

    if (font_face) {
      font_face->update_access();
    }

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
  const Font& originalFontData, IsForPlatformFont, PreferColoredFont preferColoredFont, 
  const UChar* characters, unsigned length)
{
  ProfiledZone;
  ProfiledMemoryZone(MemoryTag::Font);
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
  ProfiledMemoryZone(MemoryTag::Font);
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
  ProfiledMemoryZone(MemoryTag::Font);
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

void FontCache::platformInvalidate()
{
}

std::unique_ptr<FontPlatformData> FontCache::createFontPlatformData(const FontDescription& fontDescription,
    const AtomString& family, const FontCreationContext&)
{
  ProfiledZone;
  ProfiledMemoryZone(MemoryTag::Font);
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

  WTF::String font_family = family;
  if (font_family.isEmpty())
    font_family = Convert(font_loader->fallback_font());

  ultralight::FontDatabase& font_db = ultralight::FontDatabase::instance();

  ultralight::RefPtr<ultralight::FontFace> font_face = font_db.LookupFont(
    Convert(font_family), GetRawWeight(fontDescription.weight()), !!fontDescription.italic());

  if (!font_face)
    return nullptr;

  return std::make_unique<FontPlatformData>(font_face, fontDescription);;
}

std::optional<ASCIILiteral> FontCache::platformAlternateFamilyName(const String&)
{
  // TODO
  return std::nullopt;
}

void FontCache::platformPurgeInactiveFontData()
{
  ultralight::FontDatabase::instance().Recycle();
}

}  // namespace WebCore
