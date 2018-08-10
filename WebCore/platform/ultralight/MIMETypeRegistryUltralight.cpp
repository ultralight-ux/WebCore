#include "config.h"
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include <wtf/HashMap.h>

namespace WebCore {

String MIMETypeRegistry::getPreferredExtensionForMIMEType(const String& type)
{
  // TODO
  notImplemented();

  return String();
}

String MIMETypeRegistry::getMIMETypeForExtension(const String &ext)
{
  static HashMap<String, String> mimetypeMap;
  if (mimetypeMap.isEmpty()) {
    //fill with initial values
    mimetypeMap.add("txt", "text/plain");
    mimetypeMap.add("pdf", "application/pdf");
    mimetypeMap.add("ps", "application/postscript");
    mimetypeMap.add("html", "text/html");
    mimetypeMap.add("htm", "text/html");
    mimetypeMap.add("xml", "text/xml");
    mimetypeMap.add("xsl", "text/xsl");
    mimetypeMap.add("js", "application/x-javascript");
    mimetypeMap.add("xhtml", "application/xhtml+xml");
    mimetypeMap.add("rss", "application/rss+xml");
    mimetypeMap.add("webarchive", "application/x-webarchive");
    mimetypeMap.add("svg", "image/svg+xml");
    mimetypeMap.add("svgz", "image/svg+xml");
    mimetypeMap.add("jpg", "image/jpeg");
    mimetypeMap.add("jpeg", "image/jpeg");
    mimetypeMap.add("png", "image/png");
    mimetypeMap.add("tif", "image/tiff");
    mimetypeMap.add("tiff", "image/tiff");
    mimetypeMap.add("ico", "image/ico");
    mimetypeMap.add("cur", "image/ico");
    mimetypeMap.add("bmp", "image/bmp");
    mimetypeMap.add("wml", "text/vnd.wap.wml");
    mimetypeMap.add("wmlc", "application/vnd.wap.wmlc");
    mimetypeMap.add("m4a", "audio/x-m4a");
  }
  return mimetypeMap.get(ext);
}

bool MIMETypeRegistry::isApplicationPluginMIMEType(const String&)
{
  // TODO
  notImplemented();

  return false;
}

}