#include "config.h"
#include "NetworkStateNotifier.h"
#include "NotImplemented.h"

namespace WebCore {



#if USE(GLIB)
void NetworkStateNotifier::networkChangedCallback(NetworkStateNotifier* networkStateNotifier)
{
    notImplemented();
}
#endif

void NetworkStateNotifier::updateStateWithoutNotifying()
{
    notImplemented();
}

void NetworkStateNotifier::startObserving()
{
    notImplemented();
}

}
