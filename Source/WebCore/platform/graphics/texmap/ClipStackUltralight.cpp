#include "config.h"
#include "ClipStackUltralight.h"

namespace WebCore {

ClipStackUltralight::ClipStackUltralight(const IntRect& size) : m_size(size)
{
}

void ClipStackUltralight::pushClip(const FloatRoundedRect& clipRect, const TransformationMatrix& clipMatrix)
{
    m_clipRects.append(clipRect);
    m_clipMatrices.append(clipMatrix);

    // Hash the new clip rect and matrix
    size_t currentHash = clipHash();
    size_t newHash = currentHash * 31 + clipRect.hash();
    newHash = newHash * 31 + clipMatrix.hash();
    m_clipHashes.append(newHash);

    // Calculate the scissor rect and intersect with current scissor rect
    IntRect currentScissor = scissorRect();
    FloatQuad quad = clipMatrix.projectQuad(clipRect.rect());
    currentScissor.intersect(quad.enclosingBoundingBox());
    m_scissorRects.append(currentScissor);
}

void ClipStackUltralight::popClip()
{
    if (m_clipRects.isEmpty())
        return;

    m_clipRects.removeLast();
    m_clipMatrices.removeLast();
    m_clipHashes.removeLast();
    m_scissorRects.removeLast();
}

void ClipStackUltralight::applyClip(ultralight::RefPtr<ultralight::Canvas> canvas) const
{
    if (isEmpty())
        return;

    for (size_t i = 0; i < m_clipRects.size(); ++i) {
        canvas->SetClip(m_clipRects[i], false, m_clipMatrices[i]);
    }
}

} // namespace WebCore