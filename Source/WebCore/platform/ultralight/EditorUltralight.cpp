#include "config.h"
#include "Editor.h"
#include "DocumentFragment.h"
#include "NotImplemented.h"
#include "Pasteboard.h"
#include "markup.h"
#include "Frame.h"
#include "Settings.h"

namespace WebCore {

  static RefPtr<DocumentFragment> createFragmentFromPasteboardData(Pasteboard& pasteboard, Frame& frame, Range& range, bool allowPlainText, bool& chosePlainText)
  {
    chosePlainText = false;

    Vector<String> types = pasteboard.typesForLegacyUnsafeBindings();
    if (types.isEmpty())
      return nullptr;

    if (types.contains("text/html;charset=utf-8") && frame.document()) {
      String markup = pasteboard.readString("text/html;charset=utf-8");
      if (RefPtr<DocumentFragment> fragment = createFragmentFromMarkup(*frame.document(), markup, emptyString(), DisallowScriptingAndPluginContent))
        return fragment;
    }

    if (!allowPlainText)
      return nullptr;

    if (types.contains("text/plain;charset=utf-8")) {
      chosePlainText = true;
      if (RefPtr<DocumentFragment> fragment = createFragmentFromText(range, pasteboard.readString("text/plain;charset=utf-8")))
        return fragment;
    }

    return nullptr;
  }

  void Editor::pasteWithPasteboard(Pasteboard* pasteboard, OptionSet<PasteOption> options)
  {
    RefPtr<Range> range = selectedRange();
    if (!range)
      return;

    bool chosePlainText;
    RefPtr<DocumentFragment> fragment = createFragmentFromPasteboardData(*pasteboard, m_frame, *range, options.contains(PasteOption::AllowPlainText), chosePlainText);

    if (fragment && options.contains(PasteOption::AsQuotation))
      quoteFragmentForPasting(*fragment);

    if (fragment && shouldInsertFragment(*fragment, range.get(), EditorInsertAction::Pasted))
      pasteAsFragment(*fragment, canSmartReplaceWithPasteboard(*pasteboard), chosePlainText, options.contains(PasteOption::IgnoreMailBlockquote) ? MailBlockquoteHandling::IgnoreBlockquote : MailBlockquoteHandling::RespectBlockquote);
  }

  RefPtr<DocumentFragment> Editor::webContentFromPasteboard(Pasteboard& pasteboard, Range& range, bool allowPlainText, bool& chosePlainText)
  {
    return createFragmentFromPasteboardData(pasteboard, m_frame, range, allowPlainText, chosePlainText);
  }

  void Editor::writeSelectionToPasteboard(Pasteboard& pasteboard)
  {
    PasteboardWebContent pasteboardContent;
    pasteboardContent.text = selectedTextForDataTransfer();
    pasteboardContent.markup = serializePreservingVisualAppearance(m_frame.selection().selection(), ResolveURLs::YesExcludingLocalFileURLsForPrivacy,
      m_frame.settings().selectionAcrossShadowBoundariesEnabled() ? SerializeComposedTree::Yes : SerializeComposedTree::No);
    pasteboard.write(pasteboardContent);
  }

  void Editor::writeImageToPasteboard(Pasteboard &, Element &, WTF::URL const &, WTF::String const &)
  {
	  // TODO
	  notImplemented();
  }

} // namespace WebCore