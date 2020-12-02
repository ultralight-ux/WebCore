
#include "config.h"
#include "SharedBuffer.h"
#include "NotImplemented.h"

#include <fstream>
#include <vector>

namespace WebCore {

  RefPtr<SharedBuffer> SharedBuffer::createFromReadingFile(const String& filePath)
  {
#if defined(WINDOWS_DESKTOP_PLATFORM)
    std::ifstream file(filePath.ascii().data(), std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size))
    {
      return SharedBuffer::create((unsigned char*)buffer.data(), buffer.size());
    }
#endif

    return nullptr;
  }

} // namespace WebCore
