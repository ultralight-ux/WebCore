#include "config.h"
#include "SynchronousLoaderClient.h"
#include "NotImplemented.h"

namespace WebCore {

  void SynchronousLoaderClient::didReceiveAuthenticationChallenge(ResourceHandle* handle, const AuthenticationChallenge& challenge)
  {
    // TODO
    notImplemented();
  }

  ResourceError SynchronousLoaderClient::platformBadResponseError()
  {
    // TODO
    notImplemented();
    return ResourceError();
  }

} // namespace WebCore