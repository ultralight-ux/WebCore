#include "config.h"
#include "Pasteboard.h"
#include "NotImplemented.h"
#include "DocumentFragment.h"

#if PLATFORM(ULTRALIGHT)

namespace WebCore {

std::unique_ptr<Pasteboard> Pasteboard::createForCopyAndPaste()
{
  notImplemented();
  // TODO
  auto pasteboard = std::make_unique<Pasteboard>();
  return pasteboard;
}

std::unique_ptr<Pasteboard> Pasteboard::createPrivate()
{
  return createForCopyAndPaste();
}

Pasteboard::Pasteboard() {
}

bool Pasteboard::hasData() { return false; }
Vector<String> Pasteboard::types() { return Vector<String>(); }
String Pasteboard::readString(const String& type) { return String(); }

void Pasteboard::writeString(const String& type, const String& data) {}
void Pasteboard::clear() {}
void Pasteboard::clear(const String& type) {}

void Pasteboard::read(PasteboardPlainText&) {}
void Pasteboard::read(PasteboardWebContentReader&) {}

void Pasteboard::write(const PasteboardURL&) {}
void Pasteboard::writeTrustworthyWebURLsPboardType(const PasteboardURL&) {}
void Pasteboard::write(const PasteboardImage&) {}
void Pasteboard::write(const PasteboardWebContent&) {}

Vector<String> Pasteboard::readFilenames() { return Vector<String>(); }
bool Pasteboard::canSmartReplace() { return false; }

void Pasteboard::writeMarkup(const String& markup) {}
void Pasteboard::writePlainText(const String&, SmartReplaceOption) {}
void Pasteboard::writePasteboard(const Pasteboard& sourcePasteboard) {}

#if ENABLE(DRAG_SUPPORT)
std::unique_ptr<Pasteboard> Pasteboard::createForDragAndDrop() {
  // TODO
  notImplemented();
  auto pasteboard = std::make_unique<Pasteboard>();
  return pasteboard;
}

std::unique_ptr<Pasteboard> Pasteboard::createForDragAndDrop(const DragData&) {
  // TODO
  notImplemented();
  auto pasteboard = std::make_unique<Pasteboard>();
  return pasteboard;
}

void Pasteboard::setDragImage(DragImageRef, const IntPoint& hotSpot) {
}
#endif

RefPtr<DocumentFragment> Pasteboard::documentFragment(Frame&, Range&, bool allowPlainText, bool& chosePlainText) { return nullptr; }
void Pasteboard::writeImage(Element&, const URL&, const String& title) {}
void Pasteboard::writeSelection(Range&, bool canSmartCopyOrDelete, Frame&, ShouldSerializeSelectedTextForDataTransfer) {}

} // namespace WebCore

#endif // PLATFORM(ULTRALIGHT)
