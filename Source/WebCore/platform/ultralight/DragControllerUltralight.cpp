#include "config.h"
#include "DragController.h"

#include "DataTransfer.h"
#include "Document.h"
#include "DragData.h"
#include "Element.h"
#include "Pasteboard.h"
#include "markup.h"

#include "NotImplemented.h"

namespace WebCore {

const int DragController::MaxOriginalImageArea = 1500 * 1500;
const int DragController::DragIconRightInset = 7;
const int DragController::DragIconBottomInset = 3;

const float DragController::DragImageAlpha = 0.75f;

Optional<DragOperation> DragController::dragOperation(const DragData& dragData)
{
    // FIXME: To match the macOS behaviour we should return WTF::nullopt.
    // If we are a modal window, we are the drag source, or the window is an attached sheet.
    // If this can be determined from within WebCore operationForDrag can be pulled into
    // WebCore itself.
    if (dragData.containsURL() && !m_didInitiateDrag)
        return DragOperation::Copy;
    return WTF::nullopt;
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

void DragController::declareAndWriteDragImage(DataTransfer& dataTransfer, Element& element, const URL& url, const String& label)
{
    notImplemented();
}

}
