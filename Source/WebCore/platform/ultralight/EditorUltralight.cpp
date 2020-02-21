#include "config.h"
#include "Editor.h"
#include "DocumentFragment.h"
#include "NotImplemented.h"

namespace WebCore {

  void Editor::pasteWithPasteboard(Pasteboard*, OptionSet<PasteOption>)
  {
    // TODO
    notImplemented();
  }

  RefPtr<DocumentFragment> Editor::webContentFromPasteboard(Pasteboard& pasteboard, Range&, bool /*allowPlainText*/, bool& /*chosePlainText*/)
  {
    // TODO
    notImplemented();
    return nullptr;
  }

  void Editor::writeSelectionToPasteboard(Pasteboard &)
  {
	  // TODO
	  notImplemented();
  }

  void Editor::writeImageToPasteboard(Pasteboard &, Element &, WTF::URL const &, WTF::String const &)
  {
	  // TODO
	  notImplemented();
  }

} // namespace WebCore