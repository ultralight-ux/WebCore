#include "config.h"
#include "Editor.h"
#include "DocumentFragment.h"
#include "NotImplemented.h"

namespace WebCore {

  void Editor::pasteWithPasteboard(Pasteboard* pasteboard, bool allowPlainText, MailBlockquoteHandling mailBlockquoteHandling)
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

} // namespace WebCore