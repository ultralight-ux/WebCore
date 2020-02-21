#include "config.h"
#include "Image.h"
#include "BitmapImage.h"
#include "NotImplemented.h"
#include <iostream>

namespace WebCore {

  void BitmapImage::invalidatePlatformData()
  {
    // TODO
    notImplemented();
  }

  Ref<Image> Image::loadPlatformResource(const char *name)
  {
    // TODO
    notImplemented();
	WTFLogAlways("WARNING: trying to load platform resource '%s'", name);
	return BitmapImage::create();
  }

} // namespace WebCore