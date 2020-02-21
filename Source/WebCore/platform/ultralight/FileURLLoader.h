#pragma once

#include <functional>
#include <wtf/Optional.h>
#include <wtf/URL.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class SharedBuffer;

namespace FileURLLoader {

struct Result {
  String mimeType;
  String charset;
  String contentType;
  RefPtr<SharedBuffer> data;
};

using LoadCompletionHandler = WTF::Function<void(Optional<Result>)>;

void load(const URL&, LoadCompletionHandler&&);

}

}
