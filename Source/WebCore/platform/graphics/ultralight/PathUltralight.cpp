#pragma once

#include "config.h"
#include "Path.h"

#if USE(ULTRALIGHT)

#include "AffineTransform.h"
#include "FloatRect.h"
#include "GraphicsContext.h"
#include "GraphicsContextImpl.h"
#include "StrokeStyleApplier.h"
#include "RefPtrUltralight.h"
#include <Ultralight/private/Path.h>
#include <math.h>
#include <wtf/MathExtras.h>
#include <wtf/text/WTFString.h>

#include "NotImplemented.h"

namespace WebCore {

class UltralightPath {
public:
  UltralightPath() : m_path(ultralight::Path::Create()) {}
  ~UltralightPath() {}

  ultralight::RefPtr<ultralight::Path> path() const { return m_path; }

private:
  ultralight::RefPtr<ultralight::Path> m_path;
  friend class Path;
};

Path::Path()
  : m_path(nullptr)
{
}

Path::~Path()
{
  if (m_path)
    delete m_path;
}

Path::Path(const Path& other)
  : m_path(0)
{
  if (other.isNull())
    return;

  auto path = ensurePlatformPath()->path();
  path->Set(other.platformPath()->path());
}

Path::Path(Path&& other)
{
	m_path = other.m_path;
	other.m_path = nullptr;
}

Path& Path::operator=(Path&& other)
{
	if (this == &other)
		return *this;
	if (m_path)
		delete m_path;
	m_path = other.m_path;
	other.m_path = nullptr;
	return *this;
}

PlatformPathPtr Path::ensurePlatformPath() const
{
  if (!m_path)
    m_path = new UltralightPath();
  return m_path;
}

Path& Path::operator=(const Path& other)
{
  if (&other == this)
    return *this;

  if (other.isNull()) {
    if (m_path) {
      delete m_path;
      m_path = 0;
    }
  }
  else {
    auto path = ensurePlatformPath()->path();
    path->Set(other.platformPath()->path());
  }

  return *this;
}

void Path::clear()
{
  if (isNull())
    return;

  auto path = ensurePlatformPath()->path();
  path->Clear();
}

bool Path::isEmpty() const
{
  return isNull() || platformPath()->path()->empty();
}

bool Path::hasCurrentPoint() const
{
  if (isNull())
    return false;

  return platformPath()->path()->has_current_point();
}

FloatPoint Path::currentPoint() const
{
  if (isNull())
    return FloatPoint();

  ultralight::Point p = platformPath()->path()->current_point();
  return FloatPoint(p.x, p.y);
}

void Path::translate(const FloatSize& p)
{
  transform(AffineTransform(1, 0, 0, 1, p.width(), p.height()));
}

void Path::moveTo(const FloatPoint& p)
{
  auto path = ensurePlatformPath()->path();
  path->MoveTo({ p.x(), p.y() });
}

void Path::addLineTo(const FloatPoint& p)
{
  auto path = ensurePlatformPath()->path();
  path->LineTo({ p.x(), p.y() });
}

void Path::addRect(const FloatRect& rect)
{
  auto path = ensurePlatformPath()->path();
  // Draw clockwise rectangle with LineTo
  path->MoveTo({ rect.x(), rect.y() });
  path->LineTo({ rect.x() + rect.width(), rect.y() });
  path->LineTo({ rect.x() + rect.width(), rect.y() + rect.height() });
  path->LineTo({ rect.x(), rect.y() + rect.height() });
  path->LineTo({ rect.x(), rect.y() });
  path->Close();
}

void Path::addQuadCurveTo(const FloatPoint& controlPoint, const FloatPoint& point)
{
  auto path = ensurePlatformPath()->path();
  path->ConicTo({ controlPoint.x(), controlPoint.y() }, 
                { point.x(), point.y() });
}

void Path::addBezierCurveTo(const FloatPoint& controlPoint1, const FloatPoint& controlPoint2, const FloatPoint& controlPoint3)
{
  auto path = ensurePlatformPath()->path();
  path->CubicTo({ controlPoint1.x(), controlPoint1.y() },
                { controlPoint2.x(), controlPoint2.y() },
                { controlPoint3.x(), controlPoint3.y() });
}

void Path::addArc(const FloatPoint& p, float r, float startAngle, float endAngle, bool anticlockwise)
{
  if (!std::isfinite(r) || !std::isfinite(startAngle) || !std::isfinite(endAngle))
    return;

  auto path = ensurePlatformPath()->path();
  float sweep = endAngle - startAngle;
  const float twoPI = 2 * piFloat;
  if ((sweep <= -twoPI || sweep >= twoPI)
    && ((anticlockwise && (endAngle < startAngle)) || (!anticlockwise && (startAngle < endAngle)))) {
    if (anticlockwise)
      path->ArcTo({ p.x(), p.y() }, r, startAngle, startAngle - twoPI, true);
    else
      path->ArcTo({ p.x(), p.y() }, r, startAngle, startAngle + twoPI, false);
    path->Close();
    path->ArcTo({ p.x(), p.y() }, r, endAngle, endAngle, false);
  }
  else {
    if (anticlockwise)
      path->ArcTo({ p.x(), p.y() }, r, startAngle, endAngle, true);
    else
      path->ArcTo({ p.x(), p.y() }, r, startAngle, endAngle, false);
  }
}

static inline float areaOfTriangleFormedByPoints(const FloatPoint& p1, const FloatPoint& p2, const FloatPoint& p3)
{
  return p1.x() * (p2.y() - p3.y()) + p2.x() * (p3.y() - p1.y()) + p3.x() * (p1.y() - p2.y());
}

void Path::addArcTo(const FloatPoint& p1, const FloatPoint& p2, float radius)
{
  auto path = ensurePlatformPath()->path();

  ultralight::Point cur_p = path->current_point();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
  FloatPoint p0(cur_p.x, cur_p.y);

  // Draw only a straight line to p1 if any of the points are equal or the radius is zero
  // or the points are collinear (triangle that the points form has area of zero value).
  if ((p1.x() == p0.x() && p1.y() == p0.y()) || (p1.x() == p2.x() && p1.y() == p2.y()) || !radius
    || !areaOfTriangleFormedByPoints(p0, p1, p2)) {
    path->LineTo({ p1.x(), p1.y() });
    return;
  }

  FloatPoint p1p0((p0.x() - p1.x()), (p0.y() - p1.y()));
  FloatPoint p1p2((p2.x() - p1.x()), (p2.y() - p1.y()));
  float p1p0_length = sqrtf(p1p0.x() * p1p0.x() + p1p0.y() * p1p0.y());
  float p1p2_length = sqrtf(p1p2.x() * p1p2.x() + p1p2.y() * p1p2.y());

  double cos_phi = (p1p0.x() * p1p2.x() + p1p0.y() * p1p2.y()) / (p1p0_length * p1p2_length);
  // all points on a line logic
  if (cos_phi == -1) {
    path->LineTo({ p1.x(), p1.y() });
    return;
  }
  if (cos_phi == 1) {
    // add infinite far away point
    unsigned int max_length = 65535;
    double factor_max = max_length / p1p0_length;
    FloatPoint ep((p0.x() + factor_max * p1p0.x()), (p0.y() + factor_max * p1p0.y()));
    path->LineTo({ ep.x(), ep.y() });
    return;
  }

  float tangent = radius / tan(acos(cos_phi) / 2);
  float factor_p1p0 = tangent / p1p0_length;
  FloatPoint t_p1p0((p1.x() + factor_p1p0 * p1p0.x()), (p1.y() + factor_p1p0 * p1p0.y()));

  FloatPoint orth_p1p0(p1p0.y(), -p1p0.x());
  float orth_p1p0_length = sqrt(orth_p1p0.x() * orth_p1p0.x() + orth_p1p0.y() * orth_p1p0.y());
  float factor_ra = radius / orth_p1p0_length;

  // angle between orth_p1p0 and p1p2 to get the right vector orthographic to p1p0
  double cos_alpha = (orth_p1p0.x() * p1p2.x() + orth_p1p0.y() * p1p2.y()) / (orth_p1p0_length * p1p2_length);
  if (cos_alpha < 0.f)
    orth_p1p0 = FloatPoint(-orth_p1p0.x(), -orth_p1p0.y());

  FloatPoint p((t_p1p0.x() + factor_ra * orth_p1p0.x()), (t_p1p0.y() + factor_ra * orth_p1p0.y()));

  // calculate angles for addArc
  orth_p1p0 = FloatPoint(-orth_p1p0.x(), -orth_p1p0.y());
  float sa = acos(orth_p1p0.x() / orth_p1p0_length);
  if (orth_p1p0.y() < 0.f)
    sa = 2 * piDouble - sa;

  // anticlockwise logic
  bool anticlockwise = false;

  float factor_p1p2 = tangent / p1p2_length;
  FloatPoint t_p1p2((p1.x() + factor_p1p2 * p1p2.x()), (p1.y() + factor_p1p2 * p1p2.y()));
  FloatPoint orth_p1p2((t_p1p2.x() - p.x()), (t_p1p2.y() - p.y()));
  float orth_p1p2_length = sqrtf(orth_p1p2.x() * orth_p1p2.x() + orth_p1p2.y() * orth_p1p2.y());
  float ea = acos(orth_p1p2.x() / orth_p1p2_length);
  if (orth_p1p2.y() < 0)
    ea = 2 * piDouble - ea;
  if ((sa > ea) && ((sa - ea) < piDouble))
    anticlockwise = true;
  if ((sa < ea) && ((ea - sa) > piDouble))
    anticlockwise = true;

  path->LineTo({ t_p1p0.x(), t_p1p0.y() });

  addArc(p, radius, sa, ea, anticlockwise);
}

void Path::addEllipse(FloatPoint point, float radiusX, float radiusY, float rotation, float startAngle, float endAngle, bool anticlockwise)
{
  auto path = ensurePlatformPath()->path();
  auto& matrix = path->matrix();
  ultralight::Matrix old_matrix = matrix;
  matrix.Translate(point.x(), point.y());
  matrix.Rotate(rotation, 0.0f);
  matrix.Scale(radiusX, radiusY);
  path->ArcTo({ 0.0f, 0.0f }, 1.0f, startAngle, endAngle, anticlockwise);
  matrix = old_matrix;
}

void Path::addEllipse(const FloatRect& rect)
{
  auto path = ensurePlatformPath()->path();
  auto& matrix = path->matrix();
  ultralight::Matrix old_matrix = matrix;
  float yRadius = .5 * rect.height();
  float xRadius = .5 * rect.width();
  matrix.Translate(rect.x() + xRadius, rect.y() + yRadius);
  matrix.Scale(xRadius, yRadius);
  path->ArcTo({ 0.0f, 0.0f, }, 1.0f, 0.0f, 2.0f * piDouble, false);
  matrix = old_matrix;
}

void Path::addPath(const Path& path, const AffineTransform& transform)
{
  notImplemented();
}

void Path::closeSubpath()
{
  auto path = ensurePlatformPath()->path();
  path->Close();
}

FloatRect Path::boundingRect() const
{
  // Should this be isEmpty() or can an empty path have a non-zero origin?
  if (isNull())
    return FloatRect();

  auto aabb = platformPath()->path()->aabb();
  return FloatRect(aabb.x(), aabb.y(), aabb.width(), aabb.height());
}

FloatRect Path::strokeBoundingRect(StrokeStyleApplier* applier) const
{
  // Should this be isEmpty() or can an empty path have a non-zero origin?
  if (isNull())
    return FloatRect();

  if (!applier)
    return FloatRect();

  GraphicsContext gc;
  applier->strokeStyle(&gc);
  float stroke_width = gc.strokeThickness();
  FloatRect stroked_bounds = boundingRect();
  stroked_bounds.inflate(stroke_width);
  return stroked_bounds;
}

bool Path::contains(const FloatPoint& point, WindRule rule) const
{
  if (isNull() || !std::isfinite(point.x()) || !std::isfinite(point.y()))
    return false;
  auto path = platformPath()->path();
  ultralight::FillRule fill_rule = rule == WindRule::NonZero ? ultralight::kFillRule_NonZero : ultralight::kFillRule_EvenOdd;
  return path->IsPointFilled({ point.x(), point.y() }, fill_rule);
}

bool Path::strokeContains(StrokeStyleApplier* applier, const FloatPoint& point) const
{
  if (isNull())
    return false;

  if (!applier)
    return false;

  auto path = platformPath()->path();
  GraphicsContext gc;
  applier->strokeStyle(&gc);
  float stroke_width = gc.strokeThickness();
  float dist = path->GetDistanceToPoint({ point.x(), point.y() });
  return dist < stroke_width;
}

void PathApplyVisitor(void* context, ultralight::PathCommand command, float x1, float y1, float x2, float y2) {
  const PathApplierFunction& apply_fn = *(const PathApplierFunction*)context;
  PathElement pelement;
  FloatPoint points[3];
  pelement.points = points;

  switch (command) {
  case ultralight::kPathCommand_MoveTo:
    pelement.type = PathElementMoveToPoint;
    pelement.points[0] = FloatPoint(x1, y1);
    apply_fn(pelement);
    break;
  case ultralight::kPathCommand_QuadraticTo:
    pelement.type = PathElementAddQuadCurveToPoint;
    pelement.points[0] = FloatPoint(x1, y1);
    pelement.points[1] = FloatPoint(x2, y2);
    apply_fn(pelement);
    break;
  case ultralight::kPathCommand_Close:
    pelement.type = PathElementCloseSubpath;
    apply_fn(pelement);
    break;
  }
}

void Path::apply(const PathApplierFunction& function) const
{
  if (isNull())
    return;

  auto path = platformPath()->path();
  path->GetCommands(PathApplyVisitor, (void*)&function);
}

void Path::transform(const AffineTransform& trans)
{
  auto path = ensurePlatformPath()->path();

  // TODO: the Cairo port inverts the matrix right here, should we do the same??

  path->Transform(trans);
}

ultralight::RefPtr<ultralight::Path> Path::ultralightPath() const {
  return ensurePlatformPath()->path();
}

} // namespace WebCore

#endif  // USE(ULTRALIGHT)
