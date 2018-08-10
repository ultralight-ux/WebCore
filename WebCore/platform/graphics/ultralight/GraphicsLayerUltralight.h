#pragma once
#include "NotImplemented.h"
#include "GraphicsLayerFactory.h"
#include "Image.h"
#include "GraphicsLayerTransform.h"
#include <ultralight/private/Canvas.h>

namespace WebCore {

class GraphicsLayerUltralight : public GraphicsLayer {
public:
  GraphicsLayerUltralight(Type type, GraphicsLayerClient& client);

  virtual ~GraphicsLayerUltralight();

  virtual bool setChildren(const Vector<GraphicsLayer*>& children);
  virtual void addChild(GraphicsLayer* layer) override;
  virtual void addChildAtIndex(GraphicsLayer* layer, int index) override;
  virtual void addChildAbove(GraphicsLayer* layer, GraphicsLayer* sibling) override;
  virtual void addChildBelow(GraphicsLayer* layer, GraphicsLayer* sibling) override;
  virtual bool replaceChild(GraphicsLayer* oldChild, GraphicsLayer* newChild) override;
  virtual void setDrawsContent(bool value) override;
  virtual void setNeedsDisplay() override;
  virtual void setNeedsDisplayInRect(const FloatRect& rect, ShouldClipToLayer = ClipToLayer) override;
  virtual void setContentsNeedsDisplay() override;
  virtual void setContentsRect(const FloatRect& value) override;
  virtual bool addAnimation(const KeyframeValueList& valueList, const FloatSize& boxSize, const Animation* anim, const String& keyframesName, double timeOffset);
  virtual void pauseAnimation(const String&, double) override;
  virtual void removeAnimation(const String&) override;
  virtual void setContentsToImage(Image* img) override;
  virtual bool shouldDirectlyCompositeImage(Image* image) const override;
  virtual void setContentsToSolidColor(const Color& color) override;
  virtual void setContentsToPlatformLayer(PlatformLayer* layer, ContentsLayerPurpose purpose) override;
  virtual TiledBacking* tiledBacking() const override;

  // Member Functions:
  void WEBCORE_EXPORT Update();
  void WEBCORE_EXPORT ComputeTransforms();
  void WEBCORE_EXPORT Paint(ultralight::RefPtr<ultralight::Canvas> canvas);

protected:
  void UpdateSelf();
  void PaintSelf(ultralight::RefPtr<ultralight::Canvas> canvas);
  void SortByZOrder(Vector<GraphicsLayer*>& layers);

  ultralight::RefPtr<ultralight::Canvas> canvas_;
  bool m_needsDisplay = false;
  FloatRect m_needsDisplayRect;
  GraphicsLayerTransform m_currentTransform;
  bool m_visible = true;
  float m_currentOpacity = 1.0;
  float m_centerZ = 0.0;
  Color m_solidColor;
};

}  // namespace WebCore
