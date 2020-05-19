#pragma once

#if USE(ULTRALIGHT)

#include "TextFlags.h"
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/RefPtr.h>
#include <Ultralight/private/Vector.h>
#include <Ultralight/Buffer.h>
#include "RefPtrFreeTypeFace.h"
#include "FontPlatformData.h"

namespace WebCore {

  class FontDescription;
  class SharedBuffer;
  struct FontSelectionSpecifiedCapabilities;
  struct FontVariantSettings;

  template <typename T> class FontTaggedSettings;
  typedef FontTaggedSettings<int> FontFeatureSettings;

  struct FontCustomPlatformData {
  public:
    FontCustomPlatformData(ultralight::RefPtr<ultralight::FontFace>);
    FontCustomPlatformData(const FontCustomPlatformData&);
    FontCustomPlatformData(FontCustomPlatformData&&) = default;

    FontPlatformData fontPlatformData(const FontDescription&, bool bold, bool italic, const FontFeatureSettings&, const FontVariantSettings&, FontSelectionSpecifiedCapabilities);
    
    ~FontCustomPlatformData();
    
    static bool supportsFormat(const String&);

    FontCustomPlatformData& operator=(const FontCustomPlatformData&);
    FontCustomPlatformData& operator=(FontCustomPlatformData&&) = default;

  private:
    ultralight::RefPtr<ultralight::FontFace> m_face;
  };

  std::unique_ptr<FontCustomPlatformData> createFontCustomPlatformData(SharedBuffer&, const String&);

} // namespace WebCore

#endif // USE(ULTRALIGHT)
