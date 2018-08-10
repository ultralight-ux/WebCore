#include "config.h"
#include "EventHandler.h"
#include "Frame.h"
#include "FrameView.h"
#include "Page.h"
#include "MouseEventWithHitTestResults.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformWheelEvent.h"
#include "FocusController.h"
#include "NotImplemented.h"
#include "DataTransfer.h"

namespace WebCore {

#if ENABLE(DRAG_SUPPORT)
const double EventHandler::TextDragDelay = 0.0;
#endif

bool EventHandler::tabsToAllFormControls(KeyboardEvent*) const
{
  return true;
}

bool EventHandler::passMousePressEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe)
{
  subframe->eventHandler().handleMousePressEvent(mev.event());
  return true;
}

bool EventHandler::passMouseMoveEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe, HitTestResult* hoveredNode)
{
#if ENABLE(DRAG_SUPPORT)
  if (m_mouseDownMayStartDrag && !m_mouseDownWasInSubframe)
    return false;
#endif
  subframe->eventHandler().handleMouseMoveEvent(mev.event(), hoveredNode);
  return true;
}

bool EventHandler::passMouseReleaseEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe)
{
  subframe->eventHandler().handleMouseReleaseEvent(mev.event());
  return true;
}

bool EventHandler::widgetDidHandleWheelEvent(const PlatformWheelEvent& wheelEvent, Widget& widget)
{
  if (!is<FrameView>(widget))
    return false;

  return downcast<FrameView>(widget).frame().eventHandler().handleWheelEvent(wheelEvent);
}

bool EventHandler::eventActivatedView(const PlatformMouseEvent& event) const
{
  //  return event.didActivateWebView();
  // TODO
  notImplemented();
  return false;
}

#if ENABLE(DRAG_SUPPORT)
PassRefPtr<DataTransfer> EventHandler::createDraggingDataTransfer() const
{
  return DataTransfer::createForDragAndDrop();
}
#endif

void EventHandler::focusDocumentView()
{
  Page* page = m_frame.page();
  if (!page)
    return;
  page->focusController().setFocusedFrame(&m_frame);
}

bool EventHandler::passWidgetMouseDownEventToWidget(const MouseEventWithHitTestResults&)
{
  notImplemented();
  return false;
}

OptionSet<PlatformEvent::Modifier> EventHandler::accessKeyModifiers()
{
  return OptionSet<PlatformEvent::Modifier>(PlatformEvent::Modifier::AltKey);
}

}
