#include "config.h"
#include "GraphicsLayerUltralight.h"
#include "GraphicsContext.h"

namespace WebCore {

GraphicsLayerUltralight::GraphicsLayerUltralight(Type type, GraphicsLayerClient& client) : GraphicsLayer(type, client) {}

GraphicsLayerUltralight::~GraphicsLayerUltralight() {
  willBeDestroyed();
}

bool GraphicsLayerUltralight::setChildren(const Vector<GraphicsLayer*>& children) {
  return GraphicsLayer::setChildren(children);
}

void GraphicsLayerUltralight::addChild(GraphicsLayer* layer) {
  GraphicsLayer::addChild(layer);
}

void GraphicsLayerUltralight::addChildAtIndex(GraphicsLayer* layer, int index) {
  GraphicsLayer::addChildAtIndex(layer, index);
}

void GraphicsLayerUltralight::addChildAbove(GraphicsLayer* layer, GraphicsLayer* sibling) {
  GraphicsLayer::addChildAbove(layer, sibling);
}

void GraphicsLayerUltralight::addChildBelow(GraphicsLayer* layer, GraphicsLayer* sibling) {
  GraphicsLayer::addChildBelow(layer, sibling);
}

bool GraphicsLayerUltralight::replaceChild(GraphicsLayer* oldChild, GraphicsLayer* newChild) {
  return GraphicsLayer::replaceChild(oldChild, newChild);
}

void GraphicsLayerUltralight::setDrawsContent(bool value) {
  if (value == drawsContent())
    return;

  GraphicsLayer::setDrawsContent(value);

  if (value)
    setNeedsDisplay();
}

void GraphicsLayerUltralight::setNeedsDisplay() {
  if (!drawsContent())
    return;

  m_needsDisplay = true;
  addRepaintRect(FloatRect(FloatPoint(), m_size));
}

void GraphicsLayerUltralight::setNeedsDisplayInRect(const FloatRect& rect, ShouldClipToLayer) {
  if (!drawsContent())
    return;

  if (m_needsDisplay)
    return;
  m_needsDisplayRect.unite(rect);
  addRepaintRect(rect);
}

void GraphicsLayerUltralight::setContentsNeedsDisplay() {
  addRepaintRect(contentsRect());
}

void GraphicsLayerUltralight::setContentsRect(const FloatRect& value) {
  if (value == contentsRect())
    return;
  GraphicsLayer::setContentsRect(value);
}

bool GraphicsLayerUltralight::addAnimation(const KeyframeValueList& valueList, const FloatSize& boxSize, const Animation* anim, const String& keyframesName, double timeOffset) {
  return false;
}

void GraphicsLayerUltralight::pauseAnimation(const String&, double) {
}

void GraphicsLayerUltralight::removeAnimation(const String&) {
}

void GraphicsLayerUltralight::setContentsToImage(Image* img) {
  img->height();
}

bool GraphicsLayerUltralight::shouldDirectlyCompositeImage(Image* image) const { return true; }

// Pass an invalid color to remove the contents layer.
void GraphicsLayerUltralight::setContentsToSolidColor(const Color& color) { m_solidColor = color; }

void GraphicsLayerUltralight::setContentsToPlatformLayer(PlatformLayer* layer, ContentsLayerPurpose purpose) {
}

TiledBacking* GraphicsLayerUltralight::tiledBacking() const { return 0; }

// Member Functions:

void GraphicsLayerUltralight::Update() {
  UpdateSelf();

  for (auto* child : children())
    static_cast<GraphicsLayerUltralight*>(child)->Update();
}

void GraphicsLayerUltralight::ComputeTransforms() {
  m_currentTransform.setSize(m_size);
  m_currentTransform.setPosition(m_position);
  m_currentTransform.setLocalTransform(m_transform);
  m_currentTransform.setChildrenTransform(m_childrenTransform);
  m_currentTransform.setAnchorPoint(m_anchorPoint);

  if (m_parent) {
    m_currentTransform.combineTransforms(
      static_cast<GraphicsLayerUltralight*>(m_parent)->m_currentTransform.combinedForChildren());
  }

  m_visible = backfaceVisibility() || !m_currentTransform.combined().isBackFaceVisible();

  if (m_parent && m_parent->preserves3D())
    m_centerZ = m_currentTransform.combined().mapPoint(FloatPoint3D(m_size.width() / 2, m_size.height() / 2, 0)).z();

  for (auto* child : m_children) {
    auto c = static_cast<GraphicsLayerUltralight*>(child);
    ASSERT(c->m_parent == this);
    c->ComputeTransforms();
  }

  // Reorder children if needed on the way back up.
  if (preserves3D())
    SortByZOrder(m_children);
}

void GraphicsLayerUltralight::Paint(ultralight::RefPtr<ultralight::Canvas> canvas) {
  PaintSelf(canvas);

  for (auto* child : children())
    static_cast<GraphicsLayerUltralight*>(child)->Paint(canvas);
}

void GraphicsLayerUltralight::UpdateSelf() {
  if (m_size.isEmpty())
    return;

  if (m_size.width() < 1.0 || m_size.height() < 1.0)
    return;

  IntRect dirtyRect = enclosingIntRect(FloatRect(FloatPoint::zero(), m_size));
  if (!m_needsDisplay)
    dirtyRect.intersect(enclosingIntRect(m_needsDisplayRect));
  if (dirtyRect.isEmpty())
    return;

  if (!canvas_)
    canvas_ = ultralight::Canvas::CreateRecorder(m_size.width(), m_size.height(), ultralight::kBitmapFormat_RGBA8);

  canvas_->ClearCommands();
  //canvas_->Clear();
  //canvas_->SetMatrix(AffineTransform());

  WebCore::GraphicsContext gc(canvas_);
  paintGraphicsLayerContents(gc, FloatRect(FloatPoint::zero(), m_size));

  m_needsDisplay = false;
  m_needsDisplayRect = IntRect();
}

void GraphicsLayerUltralight::PaintSelf(ultralight::RefPtr<ultralight::Canvas> canvas) {
  if (!contentsAreVisible())
    return;

  TransformationMatrix transform = m_currentTransform.combined();
  FloatRect layerRect(FloatPoint::zero(), m_size);
  ASSERT(!layerRect.isEmpty());

  canvas->Save();
  canvas->SetRootMatrix(transform);

  if (m_solidColor.isValid() && !contentsRect().isEmpty() && m_solidColor.alpha()) {
    // DRAW SOLID COLOR RECT WITH TRANSFORM
    ultralight::Paint paint;
    paint.color = UltralightRGBA(m_solidColor.red(), m_solidColor.green(), m_solidColor.blue(), m_solidColor.alpha());
    canvas->DrawRect(layerRect, paint);
    return;
  }
  
  if (canvas_) {
    // DRAW CANVAS TO CANVAS WITH TRANSFORM
    canvas->Save();
    canvas_->Replay(canvas.get());
    canvas->Restore();

    if (!layerRect.isEmpty()) {
      FloatRect borderRect = layerRect;
      borderRect.move(-2, -2);
      borderRect.expand(4, 4);

      ultralight::RoundedRect outer_rect;
      outer_rect.SetEmpty();
      outer_rect.rect = borderRect;

      ultralight::RoundedRect inner_rect;
      inner_rect.SetEmpty();
      borderRect = layerRect;
      borderRect.move(2, 2);
      borderRect.contract(2, 2);
      inner_rect.rect = borderRect;

      ultralight::BorderPaint border_paint;
      border_paint.color = UltralightColorGREEN;
      border_paint.style = ultralight::kBorderStyle_Solid;

      canvas->DrawBoxDecorations(outer_rect.rect, outer_rect, inner_rect, border_paint, border_paint,
        border_paint, border_paint, UltralightColorTRANSPARENT);
    }
  }

  canvas->Restore();
}

void GraphicsLayerUltralight::SortByZOrder(Vector<GraphicsLayer*>& layers) {
  std::sort(layers.begin(), layers.end(),
    [](GraphicsLayer* a, GraphicsLayer* b) {
    return static_cast<GraphicsLayerUltralight*>(a)->m_centerZ <
      static_cast<GraphicsLayerUltralight*>(b)->m_centerZ;
  });
}

std::unique_ptr<GraphicsLayer> GraphicsLayer::create(GraphicsLayerFactory* factory, GraphicsLayerClient& client, Type type) {
  if (!factory)
    return std::make_unique<GraphicsLayerUltralight>(type, client);

  return factory->createGraphicsLayer(type, client);
}

} // namespace WebCore
