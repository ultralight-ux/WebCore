/*
 Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies)

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 */
#include "config.h"

#include "TextureMapperTile.h"

#include "Image.h"
#include "TextureMapper.h"

#if USE(ULTRALIGHT)
#include "TextureMapperUltralight.h"
#endif

namespace WebCore {

class GraphicsLayer;

void TextureMapperTile::updateContents(TextureMapper& textureMapper, Image* image, const IntRect& dirtyRect)
{
    IntRect targetRect = enclosingIntRect(m_rect);
    targetRect.intersect(dirtyRect);
    if (targetRect.isEmpty())
        return;
    IntPoint sourceOffset = targetRect.location();

    // Normalize sourceRect to the buffer's coordinates.
    sourceOffset.move(-dirtyRect.x(), -dirtyRect.y());

    // Normalize targetRect to the texture's coordinates.
    targetRect.move(-m_rect.x(), -m_rect.y());
    if (!m_texture) {
        m_texture = textureMapper.acquireTextureFromPool(targetRect.size(), image->currentFrameKnownToBeOpaque() ? 0 : BitmapTexture::SupportsAlpha);
    }

    m_texture->updateContents(image, targetRect, sourceOffset);
    m_contentsIsImage = true;
    m_repaintCount++;
}

void TextureMapperTile::updateContents(TextureMapper& textureMapper, GraphicsLayer* sourceLayer, const IntRect& dirtyRect, float scale)
{
    IntRect targetRect = enclosingIntRect(m_rect);
    IntSize tileSize = targetRect.size();
    targetRect.intersect(dirtyRect);
    if (targetRect.isEmpty())
        return;
    IntPoint sourceOffset = targetRect.location();

    // Normalize targetRect to the texture's coordinates.
    targetRect.move(-m_rect.x(), -m_rect.y());

    if (!m_texture) {
        m_texture = textureMapper.acquireTextureFromPool(tileSize);
    }

    m_texture->updateContents(sourceLayer, targetRect, sourceOffset, scale);
    m_contentsIsImage = false;
    m_repaintCount++;
}

void TextureMapperTile::paint(TextureMapper& textureMapper, const TransformationMatrix& transform, float opacity, const unsigned exposedEdges)
{
    if (texture().get()) {
        textureMapper.drawTexture(*texture().get(), rect(), transform, opacity, exposedEdges);
    }

#if USE(ULTRALIGHT)
    m_lastPaintId = static_cast<TextureMapperUltralight&>(textureMapper).paint_id();
#endif
}

#if USE(ULTRALIGHT)
void TextureMapperTile::setNeedsUpdateInRect(const IntRect& rect)
{
    IntRect newRect = rect;
    newRect.intersect(enclosingIntRect(m_rect));
    if (newRect.isEmpty())
        return;

    m_updateRect.unite(newRect);
}

void TextureMapperTile::updateContentsIfNeeded(TextureMapper& textureMapper, GraphicsLayer* sourceLayer, float scale)
{
    if (m_updateRect.isEmpty()) {
        if (!m_wasRecycled)
            return;
        
        // TextureMapperLayer is trying to paint this tile but we previously recycled the texture.
        // Force a full update and recreate the texture.
        m_wasRecycled = false;
        m_updateRect = enclosingIntRect(m_rect);
    }

    updateContents(textureMapper, sourceLayer, m_updateRect, scale);
    m_updateRect = IntRect();
}

void TextureMapperTile::recycleTextureIfNeeded(int32_t currentPaintId)
{
    constexpr uint32_t kThreshold = 10;
    uint32_t delta = currentPaintId - m_lastPaintId;
    if (m_texture && !m_contentsIsImage && delta > kThreshold) {
        m_texture = nullptr;
        m_wasRecycled = true;
    }
}
#endif

} // namespace WebCore
