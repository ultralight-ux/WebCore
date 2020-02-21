#pragma once

#include "config.h"
#include "Widget.h"
#include "FrameView.h"
#include "HostWindow.h"

namespace WebCore {

Widget::~Widget()
{
  ASSERT(!parent());
}

void Widget::show()
{
}

void Widget::hide()
{
}

void Widget::setCursor(const Cursor& cursor)
{
  FrameView* view = root();
  if (!view)
    return;
  HostWindow* window = view->hostWindow();
  window->setCursor(cursor);
}

void Widget::paint(GraphicsContext&, const IntRect&, SecurityOriginPaintPolicy)
{
}

void Widget::setFocus(bool focused)
{
}

void Widget::setIsSelected(bool)
{
}

void Widget::setFrameRect(const IntRect& rect)
{
  m_frame = rect;
}

} // namespace WebCore