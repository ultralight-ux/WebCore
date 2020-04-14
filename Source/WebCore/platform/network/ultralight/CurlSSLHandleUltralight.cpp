#include "config.h"
#include "CurlSSLHandle.h"

namespace WebCore {

static String getCACertPathEnv()
{
    return String("cacert.pem");
}

void CurlSSLHandle::platformInitialize()
{
    auto caCertPath = getCACertPathEnv();
    if (!caCertPath.isEmpty())
        setCACertPath(WTFMove(caCertPath));
}

}
