#include "config.h"
#include "CurlSSLHandle.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Logger.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/private/util/Debug.h>
#include "StringUltralight.h"
#include <wtf/FileSystem.h>

namespace WebCore {

void CurlSSLHandle::platformInitialize()
{
  auto& platform = ultralight::Platform::instance();
  auto config = platform.config();
  WTF::String resource_path = ultralight::Convert(config.resource_path);
  if (!FileSystem::fileIsDirectory(resource_path, FileSystem::ShouldFollowSymbolicLinks::Yes)) {
    WTF::String err_msg = "Error loading cacert.pem, the following path set in Config::resource_path does not exist or is not a directory: " + resource_path;
    UL_LOG_ERROR(ultralight::Convert(err_msg));
    return;
  }

  WTF::String path = FileSystem::pathByAppendingComponent(
    ultralight::Convert(config.resource_path), "cacert.pem");
  if (!FileSystem::fileExists(path)) {
    WTF::String err_msg = "Error loading cacert.pem, the following path does not exist: " + path + "\nMake sure that Config::resource_path is set to the directory that contains cacert.pem.";
    UL_LOG_ERROR(ultralight::Convert(err_msg));
    return;
  }

  WTF::String info_msg = "Using certificate chain found at: " + path + " (you can adjust this location in Config::resource_path)";
  UL_LOG_INFO(ultralight::Convert(info_msg));

  setCACertPath(WTFMove(path));
}

}
