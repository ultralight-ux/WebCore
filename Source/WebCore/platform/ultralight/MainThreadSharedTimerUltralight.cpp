#include "config.h"
#include "MainThreadSharedTimer.h"
#include "NotImplemented.h"
#include <wtf/MonotonicTime.h>

namespace WebCore {
  static bool g_is_running = false;
  static Seconds g_fire_interval_secs; // defaults to 0
  static Seconds g_last_fired_secs;    // defaults to 0

  void MainThreadSharedTimer::setFireInterval(Seconds interval_secs) {
    g_is_running = true;
    g_fire_interval_secs = interval_secs;
    g_last_fired_secs = Seconds(WTF::MonotonicTime::now().secondsSinceEpoch());
  }

  void MainThreadSharedTimer::stop() {
    g_is_running = false;
  }

  void MainThreadSharedTimer::invalidate(void) { }

#if PLATFORM(ULTRALIGHT)
  void MainThreadSharedTimer::update() {
    Seconds now = Seconds(WTF::MonotonicTime::now().secondsSinceEpoch());
    if (g_is_running && (now - g_last_fired_secs) >= g_fire_interval_secs) {
      g_last_fired_secs = now;
      fired();
    }
  }
#endif

} // namespace WebCore
