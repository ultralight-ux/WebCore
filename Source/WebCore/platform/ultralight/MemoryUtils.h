#pragma once
#include <cstdint>
#include <cstddef>

namespace WebCore {

// Internal-only structured memory statistics from WebCore subsystems.
struct WebCoreMemoryStatistics {
  // JavaScript GC heap
  uint64_t js_gc_object_space_live = 0;     // MarkedSpace::size() - bytes in live cells
  uint64_t js_gc_object_space_capacity = 0; // MarkedSpace::capacity() - committed block memory (16KB blocks)
  uint64_t js_extra_memory = 0;             // heap.extraMemorySize() - string buffers, ArrayBuffers, butterfly storage
  uint64_t js_object_count = 0;             // heap.objectCount() - total live GC objects
  uint64_t js_protected_object_count = 0;   // heap.protectedObjectCount() - C API protected refs
  uint64_t js_global_object_count = 0;      // heap.globalObjectCount() - JSGlobalObject instances
  uint64_t js_jit_bytes = 0;               // ExecutableAllocator::committedByteCount() - compiled code memory
  uint32_t js_document_count = 0;           // Document::allDocumentsMap().size() - live DOM trees
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

  WebCoreMemoryStatistics getMemoryStatistics();

  void beginSimulatedMemoryPressure();

  void endSimulatedMemoryPressure();
};

}  // namespace WebCore
