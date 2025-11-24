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

#ifndef TextureMapperTiledBackingStore_h
#define TextureMapperTiledBackingStore_h

#include "FloatRect.h"
#include "Image.h"
#include "TextureMapperBackingStore.h"
#include "TextureMapperTile.h"
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class TextureMapper;

class TextureMapperTiledBackingStore : public RefCounted<TextureMapperTiledBackingStore>, public TextureMapperBackingStore {
public:
    static Ref<TextureMapperTiledBackingStore> create() { return adoptRef(*new TextureMapperTiledBackingStore); }
    virtual ~TextureMapperTiledBackingStore() = default;

    bool isTiledBackingStore() const override { return true; }

    void paintToTextureMapper(TextureMapper&, const FloatRect&, const TransformationMatrix&, float) override;
    void drawBorder(TextureMapper&, const Color&, float borderWidth, const FloatRect&, const TransformationMatrix&) override;
    void drawRepaintCounter(TextureMapper&, int repaintCount, const Color&, const FloatRect&, const TransformationMatrix&) override;

    void updateContentsScale(float);
    void updateContents(TextureMapper&, Image*, const FloatSize&, const IntRect&);
    void updateContents(TextureMapper&, GraphicsLayer*, const FloatSize&, const IntRect&);

#if USE(ULTRALIGHT)
    void setNeedsUpdateInRect(TextureMapper& textureMapper, GraphicsLayer*, const FloatSize&, const IntRect&);
    void paintToTextureMapperWithClip(TextureMapper&, const IntSize&, const FloatRect&, const TransformationMatrix&, float);
    void updateContentsWithClip(TextureMapper&, const IntSize&, GraphicsLayer*, const FloatRect&, const TransformationMatrix&);
    void recycleTexturesIfNeeded(TextureMapper&);
#endif

    void setContentsToImage(Image* image) { m_image = image; }

private:
    TextureMapperTiledBackingStore() = default;

    void createOrDestroyTilesIfNeeded(GraphicsLayer* layer, const FloatSize& backingStoreSize, const IntSize& tileSize, bool hasAlpha, TextureMapper& textureMapper);
    bool shouldUseTiling(GraphicsLayer* layer, const FloatSize& size, TextureMapper& textureMapper) const;
    void updateContentsFromImageIfNeeded(TextureMapper&);
    TransformationMatrix adjustedTransformForRect(const FloatRect&);
    inline FloatRect rect() const
    {
        return FloatRect(FloatPoint::zero(), m_scaledSize);
    }

    Vector<TextureMapperTile> m_tiles;
    FloatSize m_size;
    FloatSize m_scaledSize; // Effective size used for tiling (consistent with tile creation)
    RefPtr<Image> m_image;
    float m_contentsScale { 1 };
    bool m_isScaleDirty { false };
};

} // namespace WebCore

#endif
