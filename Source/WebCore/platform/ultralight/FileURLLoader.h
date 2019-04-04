#pragma once

#include <functional>
#include <wtf/Optional.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class SharedBuffer;
class URL;

namespace FileURLLoader {

struct Result {
  String mimeType;
  String charset;
  String contentType;
  RefPtr<SharedBuffer> data;
};

using LoadCompletionHandler = std::function<void(std::optional<Result>)>;

void load(const URL&, LoadCompletionHandler&&);

}

}
