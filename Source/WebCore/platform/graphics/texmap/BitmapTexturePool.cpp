/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2015 Igalia S.L.
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

#if USE(TEXTURE_MAPPER_GL)
#include "BitmapTextureGL.h"
#endif

#if USE(TEXTURE_MAPPER_ULTRALIGHT)
#include "BitmapTextureUltralight.h"
#endif

namespace WebCore {

static const Seconds releaseUnusedSecondsTolerance { 3_s };
static const Seconds releaseUnusedTexturesTimerInterval { 500_ms };

#if USE(TEXTURE_MAPPER_GL)
BitmapTexturePool::BitmapTexturePool(const TextureMapperContextAttributes& contextAttributes)
    : m_contextAttributes(contextAttributes)
    , m_releaseUnusedTexturesTimer(RunLoop::current(), this, &BitmapTexturePool::releaseUnusedTexturesTimerFired)
{
}
#endif

#if USE(TEXTURE_MAPPER_ULTRALIGHT)
BitmapTexturePool::BitmapTexturePool(bool useGpu, TextureMapper* textureMapper)
    : m_useGpu(useGpu)
    , m_textureMapper(textureMapper)
    , m_releaseUnusedTexturesTimer(RunLoop::current(), this, &BitmapTexturePool::releaseUnusedTexturesTimerFired)
{
}
#endif

RefPtr<BitmapTexture> BitmapTexturePool::acquireTexture(const IntSize& size, const BitmapTexture::Flags flags)
{
#if USE(TEXTURE_MAPPER_ULTRALIGHT)
    if (size.isEmpty())
        return nullptr;
    if (size.width() < 1 || size.height() < 1)
        return nullptr;
    
    // Round to next highest multiple of 32
    int width = size.width();
    int height = size.height();
    if (width % 32)
        width += 32 - (width % 32);
    if (height % 32)
        height += 32 - (height % 32);
    IntSize paddedSize(width, height);

    // Gather all the textures that match the size and flags into a vector.
    Vector<Entry*> candidates;
    for (auto& entry : m_textures) {
        if (entry.m_texture->refCount() == 1
            && entry.m_texture->size().width() >= paddedSize.width()
            && entry.m_texture->size().height() >= paddedSize.height()
            && entry.m_texture->flags() == flags) {
            candidates.append(&entry);
        }
    }

    // Sort the candidates by area.
    std::sort(candidates.begin(), candidates.end(),
        [](Entry* a, Entry* b) {
            return a->m_texture->size().area() < b->m_texture->size().area();
        });

    // Select the first candidate.
    Entry* selectedEntry = candidates.isEmpty() ? nullptr : candidates.first();
    if (selectedEntry) {
        // We found an entry with dimensions big enough to satisfy the request, but we have to make
        // sure its excess area is within a reasonable range:

        // If the excess area is more than 100% of the requested area or more than 150K pixels we
        // won't use it to avoid wasting memory.
        auto excessArea = selectedEntry->m_texture->size().area() - size.area();
        if (excessArea > size.area() || excessArea > 150000) {
            selectedEntry = nullptr;
        }
    }

    // Otherwise, we need to create a new texture.
    if (!selectedEntry) {
        m_textures.append(Entry(createTexture(paddedSize, size, flags)));
        selectedEntry = &m_textures.last();
    }

    scheduleReleaseUnusedTextures();
    selectedEntry->markIsInUse();
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

    // Delete entries, which have been unused in releaseUnusedSecondsTolerance.
    MonotonicTime minUsedTime = MonotonicTime::now() - releaseUnusedSecondsTolerance;

    auto numRemoved = m_textures.removeAllMatching([&minUsedTime](const Entry& entry) {
        return entry.canBeReleased(minUsedTime);
    });

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

} // namespace WebCore
