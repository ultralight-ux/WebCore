#include "config.h"
#include "DragImage.h"
#include "Element.h"
#include "TextIndicator.h"

namespace WebCore {

IntSize dragImageSize(DragImageRef image)
{
  return IntSize();
}

void deleteDragImage(DragImageRef image)
{
}

DragImageRef dissolveDragImageToFraction(DragImageRef image, float)
{
  return image;
}
        
DragImageRef createDragImageIconForCachedImageFilename(const String& filename)
{
  return nullptr;
}

DragImageRef createDragImageForLink(Element&, URL& url, const String& inLabel, TextIndicatorData&, FontRenderingMode fontRenderingMode, float)
{
  return nullptr;
}

DragImageRef scaleDragImage(DragImageRef imageRef, FloatSize scale)
{
  return nullptr;
}
    
DragImageRef createDragImageFromImage(Image* img, ImageOrientationDescription)
{
  return nullptr;
}

}  // namespace WebCore

