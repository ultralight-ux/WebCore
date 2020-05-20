#include "config.h"
#include "Pasteboard.h"
#include "NotImplemented.h"
#include "DocumentFragment.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Clipboard.h>
#include "StringUltralight.h"

#if PLATFORM(ULTRALIGHT)

namespace WebCore {

std::unique_ptr<Pasteboard> Pasteboard::createForCopyAndPaste()
{
  return std::make_unique<Pasteboard>();
}

Pasteboard::Pasteboard()
{
}

bool Pasteboard::hasData()
{
  notImplemented();
  return false;
}

Vector<String> Pasteboard::typesSafeForBindings(const String&)
{
  notImplemented();
  return { };
}

Vector<String> Pasteboard::typesForLegacyUnsafeBindings()
{
  notImplemented();
  Vector<String> types;
  return types;
}

String Pasteboard::readOrigin()
{
  notImplemented(); // webkit.org/b/177633: [GTK] Move to new Pasteboard API
  return { };
}

String Pasteboard::readString(const String& type)
{
  notImplemented();
  return String();
}

String Pasteboard::readStringInCustomData(const String&)
{
  notImplemented();
  return { };
}

void Pasteboard::writeString(const String& type, const String& text)
{
  notImplemented();
}

void Pasteboard::clear()
{
  ultralight::Clipboard* clipboard = ultralight::Platform::instance().clipboard();
  if (clipboard)
    clipboard->Clear();
}

void Pasteboard::clear(const String&)
{
  notImplemented();
}

void Pasteboard::read(PasteboardPlainText& text)
{
  ultralight::Clipboard* clipboard = ultralight::Platform::instance().clipboard();
  if (clipboard) {
    text.text = ultralight::Convert(clipboard->ReadPlainText());
  }
}

void Pasteboard::read(PasteboardWebContentReader&, WebContentReadingPolicy)
{
  notImplemented();
}

void Pasteboard::read(PasteboardFileReader&)
{
  notImplemented();
}

void Pasteboard::write(const PasteboardURL& url)
{
  writePlainText(url.url.string(), CannotSmartReplace);
}

void Pasteboard::writeTrustworthyWebURLsPboardType(const PasteboardURL&)
{
  notImplemented();
}

void Pasteboard::write(const PasteboardImage&)
{
  notImplemented();
}

void Pasteboard::write(const PasteboardWebContent& content)
{
  writePlainText(content.text, CannotSmartReplace);
}

Pasteboard::FileContentState Pasteboard::fileContentState()
{
  notImplemented();
  return FileContentState::NoFileOrImageData;
}

bool Pasteboard::canSmartReplace()
{
  notImplemented();
  return false;
}

void Pasteboard::writeMarkup(const String& markup)
{
  writePlainText(markup, CannotSmartReplace);
}

void Pasteboard::writePlainText(const String& text, SmartReplaceOption)
{
  ultralight::Clipboard* clipboard = ultralight::Platform::instance().clipboard();
  if (clipboard)
    clipboard->WritePlainText(ultralight::Convert(text));
}

void Pasteboard::writeCustomData(const PasteboardCustomData&)
{
  notImplemented();
}

void Pasteboard::write(const Color&)
{
  notImplemented();
}

#if ENABLE(DRAG_SUPPORT)
std::unique_ptr<Pasteboard> Pasteboard::createForDragAndDrop() {
  return std::make_unique<Pasteboard>();
}

std::unique_ptr<Pasteboard> Pasteboard::createForDragAndDrop(const DragData&) {
  return std::make_unique<Pasteboard>();
}

void Pasteboard::setDragImage(DragImage, const IntPoint& hotSpot) {
  notImplemented();
}
#endif // ENABLE(DRAG_SUPPORT)

#if PLATFORM(WIN) || PLATFORM(ULTRALIGHT)
RefPtr<DocumentFragment> Pasteboard::documentFragment(Frame&, Range&, bool allowPlainText, bool& chosePlainText) {
  notImplemented();
  return nullptr;
}

void Pasteboard::writeImage(Element&, const URL&, const String& title) {
  notImplemented();
}

void Pasteboard::writeSelection(Range&, bool canSmartCopyOrDelete, Frame&, ShouldSerializeSelectedTextForDataTransfer) {
  notImplemented();
}
#endif // PLATFORM(WIN) || PLATFORM(ULTRALIGHT)

} // namespace WebCore

#endif // PLATFORM(ULTRALIGHT)
