/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2015 Igalia S.L.
 * Copyright (C) 2025 Ultralight, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "BitmapTexturePool.h"

// When enabled, prevents reusing BitmapTextures within the same frame (paint ID) on GPU.
#define DISABLE_SAME_FRAME_TEXTURE_REUSE 0

#if USE(TEXTURE_MAPPER_GL)
#include "BitmapTextureGL.h"
#endif

#if USE(TEXTURE_MAPPER_ULTRALIGHT)
#include "BitmapTextureUltralight.h"
#include "TextureMapperUltralight.h"
#endif

//#define DEBUG_LOG

namespace WebCore {

static const Seconds releaseUnusedSecondsTolerance { 2_s };
static const Seconds releaseUnusedTexturesTimerInterval { 300_ms };
static const size_t maxMemoryUsage = 150 * 1024 * 1024; // 150 MB

// Static instance registry
Lock BitmapTexturePool::s_instanceLock;
HashSet<BitmapTexturePool*> BitmapTexturePool::s_instances;

#if USE(TEXTURE_MAPPER_GL)
BitmapTexturePool::BitmapTexturePool(const TextureMapperContextAttributes& contextAttributes)
    : m_contextAttributes(contextAttributes)
    , m_releaseUnusedTexturesTimer(RunLoop::current(), this, &BitmapTexturePool::releaseUnusedTexturesTimerFired)
{
    Locker locker { s_instanceLock };
    s_instances.add(this);
}
#endif

#if USE(TEXTURE_MAPPER_ULTRALIGHT)
BitmapTexturePool::BitmapTexturePool(bool useGpu, TextureMapper* textureMapper)
    : m_useGpu(useGpu)
    , m_textureMapper(textureMapper)
    , m_releaseUnusedTexturesTimer(RunLoop::current(), this, &BitmapTexturePool::releaseUnusedTexturesTimerFired)
{
    Locker locker { s_instanceLock };
    s_instances.add(this);
}
#endif

BitmapTexturePool::~BitmapTexturePool()
{
    Locker locker { s_instanceLock };
    s_instances.remove(this);
}

RefPtr<BitmapTexture> BitmapTexturePool::acquireTexture(const IntSize& size, const BitmapTexture::Flags flags, bool needsExactSize)
{
// Uncomment this to bypass the pool and always create a new texture.
#if 0
    m_textures.append(Entry(createTexture(size, size, flags)));
    auto* entry = &m_textures.last();
    entry->markIsInUse();
    entry->m_texture->reset(size, flags);
    return entry->m_texture.copyRef();
#endif

#if USE(TEXTURE_MAPPER_ULTRALIGHT)
    if (size.isEmpty())
        return nullptr;
    if (size.width() < 1 || size.height() < 1)
        return nullptr;

    Entry* selectedEntry = nullptr;
    int width = size.width();
    int height = size.height();
    IntSize paddedSize(width, height);

    // In GPU mode, get current paint_id to prevent reusing textures from current frame
    // (GPU commands may still be pending that reference the texture's canvas)
    uint32_t currentPaintId = 0;
    if (m_useGpu && m_textureMapper)
        currentPaintId = static_cast<TextureMapperUltralight*>(m_textureMapper)->paint_id();

    if (needsExactSize) {
        selectedEntry = std::find_if(m_textures.begin(), m_textures.end(),
            [&](Entry& entry) {
                if (entry.m_texture->refCount() != 1
                    || entry.m_texture->size() != size
                    || entry.m_texture->flags() != flags)
                    return false;
                // In GPU mode, skip textures used this frame (deferred commands may still reference them)
#if DISABLE_SAME_FRAME_TEXTURE_REUSE
                if (m_useGpu && entry.m_lastPaintId == currentPaintId)
                    return false;
#endif
                return true;
            });
    } else {
        // Round to next highest multiple of 128
        if (width % 128)
            width += 128 - (width % 128);
        if (height % 128)
            height += 128 - (height % 128);
        paddedSize = IntSize(width, height);

        // Gather all the textures that match the size and flags into a vector.
        Vector<Entry*> candidates;
        for (auto& entry : m_textures) {
            if (entry.m_texture->refCount() == 1
                && entry.m_texture->size().width() >= size.width()
                && entry.m_texture->size().height() >= size.height()
                && entry.m_texture->flags() == flags) {
                // In GPU mode, skip textures used this frame (deferred commands may still reference them)
#if DISABLE_SAME_FRAME_TEXTURE_REUSE
                if (m_useGpu && entry.m_lastPaintId == currentPaintId)
                    continue;
#endif
                candidates.append(&entry);
            }
        }

        // Sort the candidates by area.
        std::sort(candidates.begin(), candidates.end(),
            [](Entry* a, Entry* b) {
                return a->m_texture->size().area() < b->m_texture->size().area();
            });

        // Select the first candidate.
        selectedEntry = candidates.isEmpty() ? nullptr : candidates.first();
        if (selectedEntry) {
            // We found an entry with dimensions big enough to satisfy the request, but we have to make
            // sure its excess area is within a reasonable range:

            unsigned int textureArea = selectedEntry->m_texture->size().area();
            unsigned int paddedArea = paddedSize.area();

            if (textureArea > paddedArea) {
                // If the excess area is more than 100% of the requested area or more than 400K pixels we
                // won't use it to avoid wasting memory.
                auto excessArea = textureArea - paddedArea;
                if (excessArea > paddedArea || excessArea > 400000) {
                    selectedEntry = nullptr;
                }
            }
        }
    }

    // Otherwise, we need to create a new texture.
    if (!selectedEntry || selectedEntry == m_textures.end()) {
        m_textures.append(Entry(createTexture(paddedSize, size, flags)));
        selectedEntry = &m_textures.last();
#ifdef DEBUG_LOG
        printf("BitmapTexturePool: [%zu MB] + texture with size %dx%d\n", currentMemoryUsage() / (1024 * 1024), paddedSize.width(), paddedSize.height());
#endif
    }

    scheduleReleaseUnusedTextures();
    selectedEntry->markIsInUse();
    selectedEntry->m_lastPaintId = currentPaintId;
    selectedEntry->m_texture->reset(size, flags);
    return selectedEntry->m_texture.copyRef();
#else
    Entry* selectedEntry = std::find_if(m_textures.begin(), m_textures.end(),
        [&](Entry& entry) {
            return entry.m_texture->refCount() == 1
                && entry.m_texture->size() == size
                && (entry.m_texture->flags() & BitmapTexture::DepthBuffer) == (flags & BitmapTexture::DepthBuffer);
        });

    if (selectedEntry == m_textures.end()) {
        m_textures.append(Entry(createTexture(flags)));
        selectedEntry = &m_textures.last();
    }

    scheduleReleaseUnusedTextures();
    selectedEntry->markIsInUse();
    return selectedEntry->m_texture.copyRef();
#endif
}

void BitmapTexturePool::scheduleReleaseUnusedTextures()
{
    if (m_releaseUnusedTexturesTimer.isActive())
        return;

    m_releaseUnusedTexturesTimer.startOneShot(releaseUnusedTexturesTimerInterval);
}

void BitmapTexturePool::releaseUnusedTexturesTimerFired()
{
    if (m_textures.isEmpty())
        return;

    bool atMemoryPressure = currentMemoryUsage() > maxMemoryUsage;

    // Delete entries, which have been unused in releaseUnusedSecondsTolerance.
    MonotonicTime minUsedTime = MonotonicTime::now() - releaseUnusedSecondsTolerance;

    auto numRemoved = m_textures.removeAllMatching([&minUsedTime, atMemoryPressure](const Entry& entry) {
        return entry.canBeReleased(minUsedTime, atMemoryPressure);
    });

#ifdef DEBUG_LOG
    if (numRemoved)
        printf("BitmapTexturePool: [%zu MB] - %zu textures\n", currentMemoryUsage() / (1024 * 1024), (size_t)numRemoved);
#endif

    if (!m_textures.isEmpty())
        scheduleReleaseUnusedTextures();
}

RefPtr<BitmapTexture> BitmapTexturePool::createTexture(const BitmapTexture::Flags flags)
{
#if USE(TEXTURE_MAPPER_GL)
    return BitmapTextureGL::create(m_contextAttributes, flags);
#elif USE(TEXTURE_MAPPER_ULTRALIGHT)
    return BitmapTextureUltralight::create(m_textureMapper, m_useGpu, flags);
#else
    UNUSED_PARAM(flags);
    return nullptr;
#endif
}

#if USE(TEXTURE_MAPPER_ULTRALIGHT)
RefPtr<BitmapTexture> BitmapTexturePool::createTexture(const IntSize& paddedSize, const IntSize& contentSize, const BitmapTexture::Flags flags)
{
    return BitmapTextureUltralight::create(m_textureMapper, paddedSize, contentSize, m_useGpu, flags);
}
#endif

size_t BitmapTexturePool::currentMemoryUsage() const
{
    size_t total = 0;
    for (const auto& entry : m_textures) {
        if (entry.m_texture)
            total += entry.m_texture->size().area() * 4;
    }
    return total;
}

size_t BitmapTexturePool::totalMemoryUsage()
{
    Locker locker { s_instanceLock };
    size_t total = 0;
    for (auto* pool : s_instances)
        total += pool->currentMemoryUsage();
    return total;
}

size_t BitmapTexturePool::totalTextureCount()
{
    Locker locker { s_instanceLock };
    size_t total = 0;
    for (auto* pool : s_instances)
        total += pool->m_textures.size();
    return total;
}

} // namespace WebCore
