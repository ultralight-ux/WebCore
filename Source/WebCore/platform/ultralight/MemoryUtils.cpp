#include "config.h"
#include "MemoryUtils.h"
#include "ResourceUsageThread.h"
#include "MemoryCache.h"
#include "FontCache.h"
#include "ShadowBlur.h"
#include "BitmapTexturePool.h"
#include "CanvasBase.h"
#include "ImageBufferUltralightGPUBackend.h"
#include "ImageBufferUltralightBackend.h"
#include <wtf/MemoryPressureHandler.h>
#include <Ultralight/private/util/Debug.h>
#include <Ultralight/private/util/FormatUtils.h>
#include <sstream>
#include <string>
#if !OS(DARWIN)
#include "CommonVM.h"
#include <JavaScriptCore/VM.h>
#endif

namespace WebCore {

using ultralight::fmtBytes;

#if OS(DARWIN)
#if ENABLE(RESOURCE_USAGE)
static ResourceUsageData gData;
#endif
#endif

MemoryUtils::MemoryUtils() {
#if OS(DARWIN)
#if ENABLE(RESOURCE_USAGE)
  ResourceUsageThread::addObserver(this, Memory, [this](const ResourceUsageData& data) {
    gData = data;
  });
#endif
#endif
}

MemoryUtils::~MemoryUtils() {
#if OS(DARWIN)
#if ENABLE(RESOURCE_USAGE)
  ResourceUsageThread::removeObserver(this);
#endif
#endif
}

#define PRINT_STATS(str, obj) \
  stream << str << fmtBytes(obj) << std::endl;

void MemoryUtils::logMemoryStatistics() {
#if ENABLE(RESOURCE_USAGE)
  std::ostringstream stream;
  stream << "Memory Usage (WebCore): " << std::endl;
#if OS(DARWIN)
  PRINT_STATS("    JavaScript:          ", gData.categories[MemoryCategory::GCHeap].totalSize() + gData.categories[MemoryCategory::GCOwned].totalSize());
  // These statistics are only available on macOS at this time because of the ability to tag memory pages
  PRINT_STATS("    JavaScript JIT:      ", gData.categories[MemoryCategory::JSJIT].totalSize());
  PRINT_STATS("    Images:              ", gData.categories[MemoryCategory::Images].totalSize());
  PRINT_STATS("    Layers:              ", gData.categories[MemoryCategory::Layers].totalSize());
  PRINT_STATS("    Page:                ", gData.categories[MemoryCategory::bmalloc].totalSize() + gData.categories[MemoryCategory::LibcMalloc].totalSize());
  //PRINT_STATS("    Other:               ", Other);
#else
  JSC::VM* vm = &commonVM();
  size_t currentGCHeapCapacity = vm->heap.blockBytesAllocated();
  size_t currentGCOwnedExtra = vm->heap.extraMemorySize();
  PRINT_STATS("    JavaScript:          ", currentGCHeapCapacity + currentGCOwnedExtra);
#endif
  UL_LOG_INFO(stream.str().c_str());
#endif
}

WebCoreMemoryStatistics MemoryUtils::getMemoryStatistics() {
  WebCoreMemoryStatistics stats;

  // JavaScript heap
#if OS(DARWIN)
#if ENABLE(RESOURCE_USAGE)
  stats.javascript_bytes = gData.categories[MemoryCategory::GCHeap].totalSize()
                         + gData.categories[MemoryCategory::GCOwned].totalSize();
#endif
#else
  JSC::VM* vm = &commonVM();
  stats.javascript_bytes = vm->heap.blockBytesAllocated() + vm->heap.extraMemorySize();
#endif

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
