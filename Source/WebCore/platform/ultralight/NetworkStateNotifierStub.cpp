#include "config.h"
#include "NetworkStateNotifier.h"
#include "NotImplemented.h"

namespace WebCore {

void NetworkStateNotifier::updateStateWithoutNotifying()
{
    notImplemented();
}

#if USE(GLIB)
void NetworkStateNotifier::networkChangedCallback(NetworkStateNotifier* networkStateNotifier)
{
    notImplemented();
}
#endif

void NetworkStateNotifier::startObserving()
{
    notImplemented();
}

}
