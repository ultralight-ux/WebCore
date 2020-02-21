#pragma once

#if USE(ULTRALIGHT)

#include "TextFlags.h"
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/RefPtr.h>
#include <Ultralight/private/Vector.h>
#include <Ultralight/Buffer.h>
#include "RefPtrFreeTypeFace.h"

namespace WebCore {

  class FontDescription;
  class FontPlatformData;
  class SharedBuffer;
  struct FontSelectionSpecifiedCapabilities;
  struct FontVariantSettings;

  template <typename T> class FontTaggedSettings;
  typedef FontTaggedSettings<int> FontFeatureSettings;

  struct FontCustomPlatformData {
  public:
    FontCustomPlatformData(RefPtr<FT_FaceRec_>, ultralight::RefPtr<ultralight::Buffer>);
    FontCustomPlatformData(const FontCustomPlatformData&);
    FontCustomPlatformData(FontCustomPlatformData&&) = default;

    FontPlatformData fontPlatformData(const FontDescription&, bool bold, bool italic, const FontFeatureSettings&, const FontVariantSettings&, FontSelectionSpecifiedCapabilities);
    
    ~FontCustomPlatformData();
    
    static bool supportsFormat(const String&);

    FontCustomPlatformData& operator=(const FontCustomPlatformData&);
    FontCustomPlatformData& operator=(FontCustomPlatformData&&) = default;

  private:
    RefPtr<FT_FaceRec_> m_face;
    ultralight::RefPtr<ultralight::Buffer> m_data;
  };

  std::unique_ptr<FontCustomPlatformData> createFontCustomPlatformData(SharedBuffer&, const String&);

} // namespace WebCore

#endif // USE(ULTRALIGHT)
