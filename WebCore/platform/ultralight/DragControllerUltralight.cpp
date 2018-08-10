#include "config.h"
#include "DragController.h"
#include "NotImplemented.h"

namespace WebCore {

const int DragController::LinkDragBorderInset = 2;
const int DragController::MaxOriginalImageArea = 1500 * 1500;
const int DragController::DragIconRightInset = 7;
const int DragController::DragIconBottomInset = 3;
const float DragController::DragImageAlpha = 0.75f;

DragOperation DragController::dragOperation(const DragData& dragData)
{
  notImplemented();
  return DragOperationNone;
}

bool DragController::isCopyKeyDown(const DragData&)
{
  notImplemented();
  return false;
}
    
const IntSize& DragController::maxDragImageSize()
{
  static const IntSize maxDragImageSize(200, 200);   
  return maxDragImageSize;
}

void DragController::cleanupAfterSystemDrag()
{
}

#if ENABLE(ATTACHMENT_ELEMENT)
void DragController::declareAndWriteAttachment(DataTransfer&, Element&, const URL&)
{
}
#endif

void DragController::declareAndWriteDragImage(DataTransfer& dataTransfer, Element& element, const URL& url, const String& label)
{
  notImplemented();
}

}  // namespace WebCore
