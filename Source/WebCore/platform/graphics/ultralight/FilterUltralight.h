#pragma once

#include "Filter.h"
#include "LengthBox.h"

namespace WebCore {

class FilterOperations;
class GraphicsContext;

// Fork of CSSFilter that doesn't take a RenderElement (doesn't support reference filter)
class FilterUltralight final : public Filter {
    WTF_MAKE_FAST_ALLOCATED;
public:
    static RefPtr<FilterUltralight> create(const FilterOperations&, OptionSet<FilterRenderingMode> preferredFilterRenderingModes, const FloatSize& filterScale, ClipOperation, const FloatRect& targetBoundingBox, const GraphicsContext& destinationContext);
    WEBCORE_EXPORT static RefPtr<FilterUltralight> create(Vector<Ref<FilterFunction>>&&);

    const Vector<Ref<FilterFunction>>& functions() const { return m_functions; }

    void setFilterRegion(const FloatRect&);

    bool hasFilterThatMovesPixels() const { return m_hasFilterThatMovesPixels; }
    bool hasFilterThatShouldBeRestrictedBySecurityOrigin() const { return m_hasFilterThatShouldBeRestrictedBySecurityOrigin; }

    FilterEffectVector effectsOfType(FilterFunction::Type) const final;

    RefPtr<FilterImage> apply(FilterImage* sourceImage, FilterResults&) final;
    FilterStyleVector createFilterStyles(const FilterStyle& sourceStyle) const final;

private:
    FilterUltralight(const FloatSize& filterScale, ClipOperation, bool hasFilterThatMovesPixels, bool hasFilterThatShouldBeRestrictedBySecurityOrigin);
    FilterUltralight(Vector<Ref<FilterFunction>>&&);

    bool buildFilterFunctions(const FilterOperations&, OptionSet<FilterRenderingMode> preferredFilterRenderingModes, const FloatRect& targetBoundingBox, const GraphicsContext& destinationContext);

    OptionSet<FilterRenderingMode> supportedFilterRenderingModes() const final;

    WTF::TextStream& externalRepresentation(WTF::TextStream&, FilterRepresentation) const final;

    bool m_hasFilterThatMovesPixels { false };
    bool m_hasFilterThatShouldBeRestrictedBySecurityOrigin { false };

    Vector<Ref<FilterFunction>> m_functions;
};

} // namespace WebCore
