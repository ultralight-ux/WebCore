#include "config.h"
#include "Editor.h"
#include "DocumentFragment.h"
#include "NotImplemented.h"
#include "Pasteboard.h"
#include "markup.h"
#include "Frame.h"
#include "WebContentReader.h"
#include "Settings.h"

namespace WebCore {

static RefPtr<DocumentFragment> createFragmentFromPasteboardData(Pasteboard& pasteboard, Frame& frame, const SimpleRange& range, bool allowPlainText, bool& chosePlainText)
{
    chosePlainText = false;

    Vector<String> types = pasteboard.typesForLegacyUnsafeBindings();
    if (types.isEmpty())
        return nullptr;

    if (types.contains("text/html;charset=utf-8"_s) && frame.document()) {
        String markup = pasteboard.readString("text/html;charset=utf-8"_s);
        return createFragmentFromMarkup(*frame.document(), markup, emptyString(), {});
    }

    if (!allowPlainText)
        return nullptr;

    if (types.contains("text/plain;charset=utf-8"_s)) {
        chosePlainText = true;
        return createFragmentFromText(range, pasteboard.readString("text/plain;charset=utf-8"_s));
    }

    return nullptr;
}

RefPtr<DocumentFragment> Editor::webContentFromPasteboard(Pasteboard& pasteboard, const SimpleRange& context, bool allowPlainText, bool& chosePlainText)
{
    WebContentReader reader(*m_document.frame(), context, allowPlainText);
    pasteboard.read(reader);
    chosePlainText = reader.madeFragmentFromPlainText;
    return WTFMove(reader.fragment);
}

void Editor::writeSelectionToPasteboard(Pasteboard& pasteboard)
{
    PasteboardWebContent pasteboardContent;
    pasteboardContent.text = selectedTextForDataTransfer();
    pasteboardContent.markup = serializePreservingVisualAppearance(m_document.selection().selection(), ResolveURLs::YesExcludingURLsForPrivacy, SerializeComposedTree::Yes, IgnoreUserSelectNone::Yes);
    pasteboard.write(pasteboardContent);
}

void Editor::writeImageToPasteboard(Pasteboard&, Element&, const URL&, const String&)
{
    notImplemented();
}

void Editor::pasteWithPasteboard(Pasteboard* pasteboard, OptionSet<PasteOption> options)
{
    auto range = selectedRange();
    if (!range)
        return;

    bool chosePlainText;
    auto fragment = createFragmentFromPasteboardData(*pasteboard, *m_document.frame(), *range, options.contains(PasteOption::AllowPlainText), chosePlainText);

    if (fragment && options.contains(PasteOption::AsQuotation))
        quoteFragmentForPasting(*fragment);

    if (fragment && shouldInsertFragment(*fragment, *range, EditorInsertAction::Pasted))
        pasteAsFragment(*fragment, canSmartReplaceWithPasteboard(*pasteboard), chosePlainText, options.contains(PasteOption::IgnoreMailBlockquote) ? MailBlockquoteHandling::IgnoreBlockquote : MailBlockquoteHandling::RespectBlockquote);
}

void Editor::platformCopyFont()
{
    // Ultralight TODO
    notImplemented();
}

void Editor::platformPasteFont()
{
    // Ultralight TODO
    notImplemented();
}

} // namespace WebCore