/*
 Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)

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

#include "TextureMapperTiledBackingStore.h"

#if USE(ULTRALIGHT)
#include "TextureMapperUltralight.h"
#include <Ultralight/private/CanvasProfiler.h>
#if defined(ENABLE_CANVAS_TRACING)
#include <wtf/text/TextStream.h>
#endif
#endif

#include "GraphicsLayer.h"
#include "ImageBuffer.h"
#include "ImageObserver.h"
#include "TextureMapper.h"
#include <cmath>

namespace WebCore {

class GraphicsLayer;

#if USE(ULTRALIGHT)
static FloatRect transformRectFromLayerToGlobalCoordinateSpace(const FloatRect& rect, const TransformationMatrix& transform, const IntSize& offset)
{
    auto transformedRect = transform.mapRect(rect);
    // Some layers are drawn on an intermediate surface and have this offset applied to convert to the
    // intermediate surface coordinates. In order to translate back to actual coordinates,
    // we have to undo it.
    transformedRect.move(-offset);
    return transformedRect;
}
#endif

void TextureMapperTiledBackingStore::updateContentsFromImageIfNeeded(TextureMapper& textureMapper)
{
    if (!m_image)
        return;

    updateContents(textureMapper, m_image.get(), m_image->size(), enclosingIntRect(m_image->rect()));

    if (m_image->imageObserver())
        m_image->imageObserver()->didDraw(*m_image);
    m_image = nullptr;
}

TransformationMatrix TextureMapperTiledBackingStore::adjustedTransformForRect(const FloatRect& targetRect)
{
    return TransformationMatrix::rectToRect(rect(), targetRect);
}

void TextureMapperTiledBackingStore::paintToTextureMapper(TextureMapper& textureMapper, const FloatRect& targetRect, const TransformationMatrix& transform, float opacity)
{
    CANVAS_TRACE_WITH_STREAM("TextureMapperTiledBackingStore::paintToTextureMapper",
                             stream << "targetRect=" << targetRect
                             << " opacity=" << opacity
                             << " tileCount=" << m_tiles.size());
    updateContentsFromImageIfNeeded(textureMapper);
    TransformationMatrix adjustedTransform = transform * adjustedTransformForRect(targetRect);
    for (auto& tile : m_tiles)
        tile.paint(textureMapper, adjustedTransform, opacity, calculateExposedTileEdges(rect(), tile.rect()));
}

void TextureMapperTiledBackingStore::drawBorder(TextureMapper& textureMapper, const Color& borderColor, float borderWidth, const FloatRect& targetRect, const TransformationMatrix& transform)
{
    TransformationMatrix adjustedTransform = transform * adjustedTransformForRect(targetRect);
    for (auto& tile : m_tiles)
        textureMapper.drawBorder(borderColor, borderWidth, tile.rect(), adjustedTransform);
}

void TextureMapperTiledBackingStore::drawRepaintCounter(TextureMapper& textureMapper, int repaintCount, const Color& borderColor, const FloatRect& targetRect, const TransformationMatrix& transform)
{
    TransformationMatrix adjustedTransform = transform * adjustedTransformForRect(targetRect);
    for (auto& tile : m_tiles)
        textureMapper.drawNumber(tile.repaintCount(), borderColor, tile.rect().location(), adjustedTransform);
}

void TextureMapperTiledBackingStore::updateContentsScale(float scale)
{
    if (m_contentsScale == scale)
        return;

    m_isScaleDirty = true;
    m_contentsScale = scale;
}

bool TextureMapperTiledBackingStore::shouldUseTiling(GraphicsLayer* layer, const FloatSize& size, TextureMapper& textureMapper) const
{
    if (!layer)
        return true; // Default to tiling if no layer info available

    // Always tile PageTiledBacking and TiledBacking layers regardless of size
    if (layer->type() == GraphicsLayer::Type::PageTiledBacking ||
        layer->type() == GraphicsLayer::Type::TiledBacking)
        return true;

    // For normal layers, check if they exceed the maximum texture size
    float scale = layer->pageScaleFactor() * layer->deviceScaleFactor();
    IntSize maxSize = textureMapper.maxTextureSize(); // Now 2048x2048

    return (size.width() * scale > maxSize.width() ||
            size.height() * scale > maxSize.height());
}

void TextureMapperTiledBackingStore::createOrDestroyTilesIfNeeded(GraphicsLayer* layer, const FloatSize& size, const IntSize& tileSize, bool hasAlpha, TextureMapper& textureMapper)
{
    CANVAS_TRACE_WITH_STREAM("TextureMapperTiledBackingStore::createOrDestroyTilesIfNeeded",
                             stream << "size=" << size
                             << " tileSize=" << tileSize
                             << " hasAlpha=" << hasAlpha
                             << " contentsScale=" << m_contentsScale
                             << " currentTileCount=" << m_tiles.size()
                             << " isScaleDirty=" << m_isScaleDirty);

    // Sanity check: sizes should be reasonable (< 100K pixels in any dimension)
    // This guards against memory corruption causing garbage values in layer size.
    static constexpr float kMaxReasonableSize = 100000.0f;
    if (size.width() > kMaxReasonableSize || size.height() > kMaxReasonableSize
        || size.width() < 0 || size.height() < 0
        || std::isnan(size.width()) || std::isnan(size.height())
        || std::isinf(size.width()) || std::isinf(size.height())) {
        ASSERT_NOT_REACHED();
        return;
    }

    if (size == m_size && !m_isScaleDirty)
        return;

    m_size = size;
    m_isScaleDirty = false;

    FloatSize scaledSize(m_size);
    if (!m_image)
        scaledSize.scale(m_contentsScale);

    m_scaledSize = scaledSize; // Store for use in rect()

    Vector<FloatRect> tileRectsToAdd;
    Vector<int> tileIndicesToRemove;
    static const size_t TileEraseThreshold = 6;

    // This method recycles tiles. We check which tiles we need to add, which to remove, and use as many
    // removable tiles as replacement for new tiles when possible.
    bool useTiling = shouldUseTiling(layer, scaledSize, textureMapper);
    CANVAS_TRACE_WITH_STREAM("TextureMapperTiledBackingStore::createOrDestroyTilesIfNeeded.tiling",
                             stream << "useTiling=" << useTiling
                             << " scaledSize=" << scaledSize);
    if (useTiling) {
        // Create multiple tiles
        for (float y = 0; y < scaledSize.height(); y += tileSize.height()) {
            for (float x = 0; x < scaledSize.width(); x += tileSize.width()) {
                FloatRect tileRect(x, y, tileSize.width(), tileSize.height());
                tileRect.intersect(rect());
                tileRectsToAdd.append(tileRect);
            }
        }
    } else {
        // Create a single "tile" covering the entire layer
        tileRectsToAdd.append(rect());
    }

    // Check which tiles need to be removed, and which already exist.
    for (int i = m_tiles.size() - 1; i >= 0; --i) {
        FloatRect oldTile = m_tiles[i].rect();
        bool existsAlready = false;

        for (int j = tileRectsToAdd.size() - 1; j >= 0; --j) {
            FloatRect newTile = tileRectsToAdd[j];
            if (oldTile != newTile)
                continue;

            // A tile that we want to add already exists, no need to add or remove it.
            existsAlready = true;
            tileRectsToAdd.remove(j);
            break;
        }

        // This tile is not needed.
        if (!existsAlready)
            tileIndicesToRemove.append(i);
    }

    // Recycle removable tiles to be used for newly requested tiles.
    for (auto& rect : tileRectsToAdd) {
        if (!tileIndicesToRemove.isEmpty()) {
            // We recycle an existing tile for usage with a new tile rect.
            TextureMapperTile& tile = m_tiles[tileIndicesToRemove.last()];
            tileIndicesToRemove.removeLast();
            tile.setRect(rect);
            tile.setRepaintCount(0);

            if (tile.texture()) {
              FloatSize texSize = tile.rect().size();
              texSize.setWidth(std::round(texSize.width()));
              texSize.setHeight(std::round(texSize.height()));

              tile.texture()->reset(IntSize(texSize), hasAlpha ? BitmapTexture::SupportsAlpha : 0);
            }
            continue;
        }

        m_tiles.append(TextureMapperTile(rect));
    }

    // Remove unnecessary tiles, if they weren't recycled.
    // We use a threshold to make sure we don't create/destroy tiles too eagerly.
    for (auto& index : tileIndicesToRemove) {
        if (m_tiles.size() <= TileEraseThreshold)
            break;
        m_tiles.remove(index);
    }

    CANVAS_TRACE_WITH_STREAM("TextureMapperTiledBackingStore::createOrDestroyTilesIfNeeded.result",
                             stream << "finalTileCount=" << m_tiles.size()
                             << " tilesCreated=" << tileRectsToAdd.size()
                             << " tilesRemoved=" << tileIndicesToRemove.size());
}

void TextureMapperTiledBackingStore::updateContents(TextureMapper& textureMapper, Image* image, const FloatSize& totalSize, const IntRect& dirtyRect)
{
    createOrDestroyTilesIfNeeded(nullptr, totalSize, textureMapper.tileSize(), !image->currentFrameKnownToBeOpaque(), textureMapper);
    for (auto& tile : m_tiles)
        tile.updateContents(textureMapper, image, dirtyRect);
}

void TextureMapperTiledBackingStore::updateContents(TextureMapper& textureMapper, GraphicsLayer* sourceLayer, const FloatSize& totalSize, const IntRect& dirtyRect)
{
    // Use layer's contentsOpaque property to determine if we need alpha support
    bool hasAlpha = sourceLayer ? !sourceLayer->contentsOpaque() : true;
    createOrDestroyTilesIfNeeded(sourceLayer, totalSize, textureMapper.tileSize(), hasAlpha, textureMapper);
    for (auto& tile : m_tiles)
        tile.updateContents(textureMapper, sourceLayer, dirtyRect, m_contentsScale);
}

#if USE(ULTRALIGHT)
void TextureMapperTiledBackingStore::setNeedsUpdateInRect(TextureMapper& textureMapper, GraphicsLayer* layer, const FloatSize& totalSize, const IntRect& rect)
{
    // Use layer's contentsOpaque property to determine if we need alpha support
    bool hasAlpha = layer ? !layer->contentsOpaque() : true;
    createOrDestroyTilesIfNeeded(layer, totalSize, textureMapper.tileSize(), hasAlpha, textureMapper);
    for (auto& tile : m_tiles) {
        if (tile.rect().intersects(rect))
            tile.setNeedsUpdateInRect(rect);
    }
}

void TextureMapperTiledBackingStore::paintToTextureMapperWithClip(TextureMapper& textureMapper, const IntSize& offset, const FloatRect& targetRect, const TransformationMatrix& transform, float opacity)
{
    ProfiledZone;
    CANVAS_TRACE_WITH_STREAM("TextureMapperTiledBackingStore::paintToTextureMapperWithClip",
                             stream << "targetRect=" << targetRect
                             << " offset=" << offset
                             << " opacity=" << opacity
                             << " tileCount=" << m_tiles.size());
    updateContentsFromImageIfNeeded(textureMapper);
    TransformationMatrix adjustedTransform = transform * adjustedTransformForRect(targetRect);
    for (auto& tile : m_tiles) {
        FloatRect globalTileRect = transformRectFromLayerToGlobalCoordinateSpace(tile.rect(), adjustedTransform, offset);
        IntRect viewBounds = static_cast<TextureMapperUltralight&>(textureMapper).bounds();
        if (globalTileRect.intersects(viewBounds)) {
            tile.paint(textureMapper, adjustedTransform, opacity, calculateExposedTileEdges(rect(), tile.rect()));
        }
    }
}

void TextureMapperTiledBackingStore::updateContentsWithClip(TextureMapper& textureMapper, const IntSize& offset, GraphicsLayer* sourceLayer, const FloatRect& layerRect, const TransformationMatrix& transform)
{
    ProfiledZone;
    CANVAS_TRACE_WITH_STREAM("TextureMapperTiledBackingStore::updateContentsWithClip",
                             stream << "layerRect=" << layerRect
                             << " offset=" << offset
                             << " contentsScale=" << m_contentsScale
                             << " tileCount=" << m_tiles.size());
    TransformationMatrix adjustedTransform = transform * adjustedTransformForRect(layerRect);
    for (auto& tile : m_tiles) {
        FloatRect globalTileRect = transformRectFromLayerToGlobalCoordinateSpace(tile.rect(), adjustedTransform, offset);
        IntRect viewBounds = static_cast<TextureMapperUltralight&>(textureMapper).bounds();
        if (globalTileRect.intersects(viewBounds)) {
            tile.updateContentsIfNeeded(textureMapper, sourceLayer, m_contentsScale);
        }
    }
}

void TextureMapperTiledBackingStore::recycleTexturesIfNeeded(TextureMapper& textureMapper)
{
    for (auto& tile : m_tiles) {
        tile.recycleTextureIfNeeded(static_cast<TextureMapperUltralight&>(textureMapper).paint_id());
    }
}
#endif

} // namespace WebCore
