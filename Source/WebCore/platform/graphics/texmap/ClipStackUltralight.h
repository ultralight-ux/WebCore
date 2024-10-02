#pragma once

#include "FloatRoundedRect.h"
#include "TransformationMatrix.h"
#include <wtf/Vector.h>
#include <Ultralight/private/Canvas.h>

namespace WebCore {

class ClipStackUltralight {
public:
    ClipStackUltralight(const IntRect& size);

    void reset(const IntRect& size);

    void pushClip(const FloatRoundedRect& clipRect, const TransformationMatrix& clipMatrix);

    void popClip();

    void applyClip(ultralight::RefPtr<ultralight::Canvas> canvas) const;

    bool isEmpty() const { return m_clipRects.isEmpty(); }

    FloatRoundedRect clipRect() const {
        if (!m_clipRects.isEmpty())
            return m_clipRects.last();
        return FloatRoundedRect();
    }

    TransformationMatrix clipMatrix() const {
        if (!m_clipMatrices.isEmpty())
            return m_clipMatrices.last();
        return TransformationMatrix();
    }

    size_t clipHash() const { 
        if (!m_clipHashes.isEmpty())
            return m_clipHashes.last();
        return 0;
    }

    IntRect scissorRect() const {
        if(!m_scissorRects.isEmpty())
            return m_scissorRects.last();
        return m_size;
    }

private:
    IntRect m_size;
    static const size_t m_initialCapacity = 4;
    Vector<FloatRoundedRect, m_initialCapacity> m_clipRects;
    Vector<TransformationMatrix, m_initialCapacity> m_clipMatrices;
    Vector<size_t, m_initialCapacity> m_clipHashes;
    Vector<IntRect, m_initialCapacity> m_scissorRects;
};

} // namespace WebCore