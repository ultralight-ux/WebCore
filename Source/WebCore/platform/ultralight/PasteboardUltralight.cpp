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
    return makeUnique<Pasteboard>();
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
    return {};
}

Vector<String> Pasteboard::typesForLegacyUnsafeBindings()
{
    notImplemented();
    return {};
}

String Pasteboard::readOrigin()
{
    notImplemented(); // webkit.org/b/177633: [GTK] Move to new Pasteboard API
    return {};
}

String Pasteboard::readString(const String& type)
{
    notImplemented();
    return {};
}

String Pasteboard::readStringInCustomData(const String&)
{
    notImplemented();
    return {};
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

void Pasteboard::read(PasteboardPlainText& text, PlainTextURLReadingPolicy, Optional<size_t>)
{
    ultralight::Clipboard* clipboard = ultralight::Platform::instance().clipboard();
    if (clipboard) {
        text.text = ultralight::Convert(clipboard->ReadPlainText());
    }
}

void Pasteboard::read(PasteboardWebContentReader&, WebContentReadingPolicy, Optional<size_t>)
{
    notImplemented();
}

void Pasteboard::read(PasteboardFileReader&, Optional<size_t>)
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

void Pasteboard::writeCustomData(const Vector<PasteboardCustomData>&)
{
    notImplemented();
}

void Pasteboard::write(const Color&)
{
    notImplemented();
}

std::unique_ptr<Pasteboard> Pasteboard::createForDragAndDrop()
{
    notImplemented();
    return makeUnique<Pasteboard>();
}
std::unique_ptr<Pasteboard> Pasteboard::createForDragAndDrop(const DragData&)
{
    notImplemented();
    return makeUnique<Pasteboard>();
}

void Pasteboard::setDragImage(DragImage, const IntPoint& hotSpot)
{
    notImplemented();
}

} // namespace WebCore

#endif // PLATFORM(ULTRALIGHT)
