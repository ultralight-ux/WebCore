#include "config.h"
#include "Image.h"
#include "BitmapImage.h"
#include "NotImplemented.h"
#include <iostream>

namespace WebCore {

  void BitmapImage::invalidatePlatformData()
  {
    // TODO
    // notImplemented();
  }

  PassRefPtr<Image> Image::loadPlatformResource(const char *name)
  {
    // TODO
    std::cerr << "TODO: Image::loadPlatformResource " << name << std::endl;
    notImplemented();
    return Image::nullImage();
  }

} // namespace WebCore