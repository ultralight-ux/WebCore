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
  return{ (float)a(), (float)b(), (float)c(), (float)d(), (float)e(), (float)f() };
}

AffineTransform::operator ultralight::Matrix() const
{
  return{ (float)a(), (float)b(), (float)c(), (float)d(), (float)e(), (float)f() };
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)


