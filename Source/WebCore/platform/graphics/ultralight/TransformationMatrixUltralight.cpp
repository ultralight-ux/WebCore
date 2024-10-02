#include "config.h"

#if USE(ULTRALIGHT)
#include "TransformationMatrix.h"
#include "AffineTransform.h"
#include "FloatRect.h"
#include "IntRect.h"
#include <Ultralight/Matrix.h>

namespace WebCore {

TransformationMatrix::operator ultralight::Matrix() const
{
  ultralight::Matrix mat;
  mat.Set(m11(), m12(), m13(), m14(), m21(), m22(), m23(), m24(),
          m31(), m32(), m33(), m34(), m41(), m42(), m43(), m44());
  return mat;
}

size_t TransformationMatrix::hash() const
{
  // Treat the matrix as an array of bytes
  const unsigned char* data = reinterpret_cast<const unsigned char*>(m_matrix);
  std::size_t size = sizeof(m_matrix);
  
  // FNV-1a hash
  std::size_t hash = 14695981039346656037ULL; // FNV offset basis
  for (std::size_t i = 0; i < size; ++i) {
      hash ^= static_cast<std::size_t>(data[i]);
      hash *= 1099511628211ULL; // FNV prime
  }
  return hash;
}

AffineTransform::operator ultralight::Matrix() const
{
  ultralight::Matrix mat;
  mat.Set(a(), b(), c(), d(), e(), f());
  return mat;
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
