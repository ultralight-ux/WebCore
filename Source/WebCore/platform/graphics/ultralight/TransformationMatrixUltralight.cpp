#pragma once

#if USE(ULTRALIGHT)

#include "config.h"
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

AffineTransform::operator ultralight::Matrix() const
{
  ultralight::Matrix mat;
  mat.Set(a(), b(), c(), d(), e(), f());
  return mat;
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)


