#pragma once
#include <cstdint>
#include <cstddef>

namespace WebCore {

// Internal-only structured memory statistics from WebCore subsystems.
struct WebCoreMemoryStatistics {
  uint64_t javascript_bytes = 0;
  // MemoryCache (resource cache)
  uint64_t cache_images_size = 0;
  uint64_t cache_images_decoded_size = 0;
  uint64_t cache_css_size = 0;
  uint64_t cache_scripts_size = 0;
  uint64_t cache_fonts_size = 0;
  // FontCache
  uint32_t font_count = 0;
  uint32_t inactive_font_count = 0;
  // Compositor pools
  uint64_t bitmap_texture_pool_bytes = 0;
  uint32_t bitmap_texture_pool_count = 0;
  uint64_t shadow_buffer_pool_bytes = 0;
  uint32_t shadow_buffer_pool_count = 0;
  // HTML5 Canvas elements
  uint32_t html_canvas_total = 0;
  uint32_t html_canvas_accelerated = 0;
  uint32_t html_canvas_unaccelerated = 0;
  // ImageBuffer backends
  uint32_t image_buffer_gpu_count = 0;
  uint64_t image_buffer_gpu_bytes = 0;
  uint32_t image_buffer_cpu_count = 0;
  uint64_t image_buffer_cpu_bytes = 0;
};

class WEBCORE_EXPORT MemoryUtils {
public:
  MemoryUtils();
  ~MemoryUtils();

  void logMemoryStatistics();

  WebCoreMemoryStatistics getMemoryStatistics();

  void beginSimulatedMemoryPressure();

  void endSimulatedMemoryPressure();
};

}  // namespace WebCore
