/*
 * Copyright (C) 2016 Apple Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Color.h"
#include "IntRect.h"
#include "IntSize.h"
#include "NativeImage.h"
#include "SharedBuffer.h"
#if USE(ULTRALIGHT)
#include <Ultralight/Geometry.h>
#include <Ultralight/Bitmap.h>
#endif

namespace WebCore {

#if USE(CAIRO)
// Due to the pixman 16.16 floating point representation, cairo is not able to handle
// images whose size is bigger than 32768.
static const int cairoMaxImageSize = 32768;
#endif

class ImageBackingStore {
  WTF_MAKE_FAST_ALLOCATED;
public:
  static std::unique_ptr<ImageBackingStore> create(const IntSize& size, bool premultiplyAlpha = true)
  {
      return std::unique_ptr<ImageBackingStore>(new ImageBackingStore(size, premultiplyAlpha));
  }

  static std::unique_ptr<ImageBackingStore> create(const ImageBackingStore& other)
  {
      return std::unique_ptr<ImageBackingStore>(new ImageBackingStore(other));
  }

  NativeImagePtr image(bool is_complete) const;

  bool setSize(const IntSize& size);

  void setFrameRect(const IntRect& frameRect);

  const IntSize& size() const { return m_size; }
  const IntRect& frameRect() const { return m_frameRect; }

  void clear();

  void clearRect(const IntRect& rect);

  void fillRect(const IntRect &rect, unsigned r, unsigned g, unsigned b, unsigned a);

  void repeatFirstRow(const IntRect& rect);

  RGBA32* pixelAt(int x, int y) const;

  void setPixel(RGBA32* dest, unsigned r, unsigned g, unsigned b, unsigned a);

  void setPixel(int x, int y, unsigned r, unsigned g, unsigned b, unsigned a);

#if ENABLE(APNG)
  void blendPixel(RGBA32* dest, unsigned r, unsigned g, unsigned b, unsigned a);
#endif

  static bool isOverSize(const IntSize& size)
  {
      static unsigned long long MaxPixels = ((1 << 29) - 1);
      unsigned long long pixels = static_cast<unsigned long long>(size.width()) * static_cast<unsigned long long>(size.height());
      return pixels > MaxPixels;
  }

private:
  ImageBackingStore(const IntSize& size, bool premultiplyAlpha = true);

  ImageBackingStore(const ImageBackingStore& other);

  bool inBounds(const IntPoint& point) const
  {
      return IntRect(IntPoint(), m_size).contains(point);
  }

  bool inBounds(const IntRect& rect) const
  {
      return IntRect(IntPoint(), m_size).contains(rect);
  }

    RGBA32 pixelValue(unsigned r, unsigned g, unsigned b, unsigned a) const;

    IntSize m_size;
    IntRect m_frameRect; // This will always just be the entire buffer except for GIF and PNG frames
    bool m_premultiplyAlpha { true };
#if USE(ULTRALIGHT)
    mutable ultralight::RefPtr<ultralight::Bitmap> m_bitmap;
    NativeImagePtr m_nativeImage { nullptr };
    mutable ultralight::vec4 m_buf;
#endif
};

}
