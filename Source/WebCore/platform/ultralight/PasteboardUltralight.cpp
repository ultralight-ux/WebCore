#include "config.h"
#include "Pasteboard.h"
#include "NotImplemented.h"
#include "DocumentFragment.h"
#include "DragData.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Clipboard.h>
#include "StringUltralight.h"
#include "PlatformStrategies.h"
#include "PasteboardStrategy.h"

#if PLATFORM(ULTRALIGHT)

namespace WebCore {

std::unique_ptr<Pasteboard> Pasteboard::createForCopyAndPaste(std::unique_ptr<PasteboardContext>&& context)
{
    return makeUnique<Pasteboard>(WTFMove(context));
}

Pasteboard::Pasteboard(std::unique_ptr<PasteboardContext>&& context)
    : m_context(WTFMove(context))
{
}

bool Pasteboard::hasData()
{
    // FIXME: Getting the list of types for this is wasteful. Do this in the UI process.
    Vector<String> types;
    platformStrategies()->pasteboardStrategy()->getTypes(types);
    return !types.isEmpty();
}

Vector<String> Pasteboard::typesSafeForBindings(const String&)
{
    notImplemented();
    return { };
}

Vector<String> Pasteboard::typesForLegacyUnsafeBindings()
{
    Vector<String> types;
    platformStrategies()->pasteboardStrategy()->getTypes(types);
    return types;
}

String Pasteboard::readOrigin()
{
    notImplemented(); // webkit.org/b/177633: [GTK] Move to new Pasteboard API
    return { };
}

String Pasteboard::readString(const String& type)
{
    return platformStrategies()->pasteboardStrategy()->readStringFromPasteboard(0, type, name(), context());
}

String Pasteboard::readStringInCustomData(const String&)
{
    notImplemented();
    return { };
}

void Pasteboard::writeString(const String& type, const String& text)
{
    platformStrategies()->pasteboardStrategy()->writeToPasteboard(type, text);
}

void Pasteboard::clear()
{
}

void Pasteboard::clear(const String&)
{
}

void Pasteboard::read(PasteboardPlainText& text, PlainTextURLReadingPolicy, std::optional<size_t>)
{
    text.text = platformStrategies()->pasteboardStrategy()->readStringFromPasteboard(0, "text/plain;charset=utf-8"_s, name(), context());
}

void Pasteboard::read(PasteboardWebContentReader&, WebContentReadingPolicy, std::optional<size_t>)
{
    notImplemented();
}

void Pasteboard::read(PasteboardFileReader&, std::optional<size_t>)
{
}

void Pasteboard::write(const PasteboardURL& url)
{
    platformStrategies()->pasteboardStrategy()->writeToPasteboard("text/plain;charset=utf-8"_s, url.url.string());
}

void Pasteboard::writeTrustworthyWebURLsPboardType(const PasteboardURL&)
{
    notImplemented();
}

void Pasteboard::write(const PasteboardImage&)
{
}

void Pasteboard::write(const PasteboardBuffer&)
{
}

void Pasteboard::write(const PasteboardWebContent& content)
{
    platformStrategies()->pasteboardStrategy()->writeToPasteboard(content);
}

Pasteboard::FileContentState Pasteboard::fileContentState()
{
    notImplemented();
    return FileContentState::NoFileOrImageData;
}

bool Pasteboard::canSmartReplace()
{
    return false;
}

void Pasteboard::writeMarkup(const String&)
{
}

void Pasteboard::writePlainText(const String& text, SmartReplaceOption)
{
    writeString("text/plain;charset=utf-8"_s, text);
}

#if ENABLE(DRAG_SUPPORT)
std::unique_ptr<Pasteboard> Pasteboard::createForDragAndDrop(std::unique_ptr<PasteboardContext>&& context)
{
    // Ultralight TODO
    notImplemented();
    return makeUnique<Pasteboard>(WTFMove(context));
}

std::unique_ptr<Pasteboard> Pasteboard::create(const DragData& dragData)
{
    // Ultralight TODO
    notImplemented();
    return makeUnique<Pasteboard>(dragData.createPasteboardContext());
}

void Pasteboard::setDragImage(DragImage, const IntPoint& hotSpot)
{
    // Ultralight TODO
    notImplemented();
}
#endif

void Pasteboard::writeCustomData(const Vector<PasteboardCustomData>&)
{
}

void Pasteboard::write(const Color&)
{
}

} // namespace WebCore

#endif // PLATFORM(ULTRALIGHT)
