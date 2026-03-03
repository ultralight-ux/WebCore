#include "config.h"
#include "MemoryUtils.h"
#include "CommonVM.h"
#include "Document.h"
#include "MemoryCache.h"
#include "FontCache.h"
#include "ShadowBlur.h"
#include "BitmapTexturePool.h"
#include "CanvasBase.h"
#include "ImageBufferUltralightGPUBackend.h"
#include "ImageBufferUltralightBackend.h"
#include <JavaScriptCore/VM.h>
#include <JavaScriptCore/MemoryStatistics.h>
#include <wtf/MemoryPressureHandler.h>

namespace WebCore {

MemoryUtils::MemoryUtils() { }

MemoryUtils::~MemoryUtils() { }

WebCoreMemoryStatistics MemoryUtils::getMemoryStatistics() {
  WebCoreMemoryStatistics stats;

  // JavaScript GC heap
  JSC::VM* vm = &commonVM();
  auto& heap = vm->heap;

  size_t extraMem = heap.extraMemorySize();
  stats.js_gc_object_space_live = heap.size() - extraMem;
  stats.js_gc_object_space_capacity = heap.capacity() - extraMem;
  stats.js_extra_memory = extraMem;
  stats.js_object_count = heap.objectCount();
  stats.js_protected_object_count = heap.protectedObjectCount();
  stats.js_global_object_count = heap.globalObjectCount();

  auto globalStats = JSC::globalMemoryStatistics();
  stats.js_jit_bytes = globalStats.JITBytes;

  stats.js_document_count = static_cast<uint32_t>(Document::allDocumentsMap().size());

  // MemoryCache (resource cache)
  auto cacheStats = MemoryCache::singleton().getStatistics();
  stats.cache_images_size = cacheStats.images.size;
  stats.cache_images_decoded_size = cacheStats.images.decodedSize;
  stats.cache_css_size = cacheStats.cssStyleSheets.size;
  stats.cache_scripts_size = cacheStats.scripts.size;
  stats.cache_fonts_size = cacheStats.fonts.size;

  // FontCache
  auto& fontCache = FontCache::forCurrentThread();
  stats.font_count = static_cast<uint32_t>(fontCache.fontCount());
  stats.inactive_font_count = static_cast<uint32_t>(fontCache.inactiveFontCount());

  // BitmapTexturePool (compositor)
  stats.bitmap_texture_pool_bytes = BitmapTexturePool::totalMemoryUsage();
  stats.bitmap_texture_pool_count = static_cast<uint32_t>(BitmapTexturePool::totalTextureCount());

  // ShadowBufferPool
  stats.shadow_buffer_pool_bytes = shadowBufferPoolMemoryUsage();
  stats.shadow_buffer_pool_count = static_cast<uint32_t>(shadowBufferPoolBufferCount());

  // HTML5 Canvas elements
  stats.html_canvas_total = static_cast<uint32_t>(CanvasBase::totalCount());
  stats.html_canvas_accelerated = static_cast<uint32_t>(CanvasBase::acceleratedCount());
  stats.html_canvas_unaccelerated = static_cast<uint32_t>(CanvasBase::unacceleratedCount());

  // ImageBuffer backends
  stats.image_buffer_gpu_count = static_cast<uint32_t>(ImageBufferUltralightGPUBackend::totalCount());
  stats.image_buffer_gpu_bytes = ImageBufferUltralightGPUBackend::totalMemoryUsage();
  stats.image_buffer_cpu_count = static_cast<uint32_t>(ImageBufferUltralightBackend::totalCount());
  stats.image_buffer_cpu_bytes = ImageBufferUltralightBackend::totalMemoryUsage();

  return stats;
}

void MemoryUtils::beginSimulatedMemoryPressure() {
  WTF::MemoryPressureHandler::singleton().beginSimulatedMemoryPressure();
}

void MemoryUtils::endSimulatedMemoryPressure() {
  WTF::MemoryPressureHandler::singleton().endSimulatedMemoryPressure();
}

}  // namespace WebCore
