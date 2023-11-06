#include "config.h"
#include "FilterUltralight.h"

#include "ColorMatrix.h"
#include "FEColorMatrix.h"
#include "FEComponentTransfer.h"
#include "FEDropShadow.h"
#include "FEGaussianBlur.h"
#include "FilterOperations.h"
#include "Logging.h"
#include "ReferencedSVGResources.h"
#include "RenderElement.h"
#include "SVGFilter.h"
#include "SVGFilterElement.h"
#include "SourceGraphic.h"

namespace WebCore {

RefPtr<FilterUltralight> FilterUltralight::create(const FilterOperations& operations, OptionSet<FilterRenderingMode> preferredFilterRenderingModes, const FloatSize& filterScale, ClipOperation clipOperation, const FloatRect& targetBoundingBox, const GraphicsContext& destinationContext)
{
    bool hasFilterThatMovesPixels = operations.hasFilterThatMovesPixels();
    bool hasFilterThatShouldBeRestrictedBySecurityOrigin = operations.hasFilterThatShouldBeRestrictedBySecurityOrigin();

    auto filter = adoptRef(*new FilterUltralight(filterScale, clipOperation, hasFilterThatMovesPixels, hasFilterThatShouldBeRestrictedBySecurityOrigin));

    if (!filter->buildFilterFunctions(operations, preferredFilterRenderingModes, targetBoundingBox, destinationContext)) {
        LOG_WITH_STREAM(Filters, stream << "FilterUltralight::create: failed to build filters " << operations);
        return nullptr;
    }

    LOG_WITH_STREAM(Filters, stream << "FilterUltralight::create built filter " << filter.get() << " for " << operations);

    filter->setFilterRenderingModes(preferredFilterRenderingModes);
    return filter;
}

RefPtr<FilterUltralight> FilterUltralight::create(Vector<Ref<FilterFunction>>&& functions)
{
    return adoptRef(new FilterUltralight(WTFMove(functions)));
}

FilterUltralight::FilterUltralight(const FloatSize& filterScale, ClipOperation clipOperation, bool hasFilterThatMovesPixels, bool hasFilterThatShouldBeRestrictedBySecurityOrigin)
    : Filter(Filter::Type::CSSFilter, filterScale, clipOperation)
    , m_hasFilterThatMovesPixels(hasFilterThatMovesPixels)
    , m_hasFilterThatShouldBeRestrictedBySecurityOrigin(hasFilterThatShouldBeRestrictedBySecurityOrigin)
{
}

FilterUltralight::FilterUltralight(Vector<Ref<FilterFunction>>&& functions)
    : Filter(Type::CSSFilter)
    , m_functions(WTFMove(functions))
{
}

static RefPtr<FilterEffect> createBlurEffect(const BlurFilterOperation& blurOperation, Filter::ClipOperation clipOperation)
{
    float stdDeviation = floatValueForLength(blurOperation.stdDeviation(), 0);
    return FEGaussianBlur::create(stdDeviation, stdDeviation, clipOperation == Filter::ClipOperation::Unite ? EdgeModeType::None : EdgeModeType::Duplicate);
}

static RefPtr<FilterEffect> createBrightnessEffect(const BasicComponentTransferFilterOperation& componentTransferOperation)
{
    ComponentTransferFunction transferFunction;
    transferFunction.type = FECOMPONENTTRANSFER_TYPE_LINEAR;
    transferFunction.slope = narrowPrecisionToFloat(componentTransferOperation.amount());
    transferFunction.intercept = 0;

    ComponentTransferFunction nullFunction;
    return FEComponentTransfer::create(transferFunction, transferFunction, transferFunction, nullFunction);
}

static RefPtr<FilterEffect> createContrastEffect(const BasicComponentTransferFilterOperation& componentTransferOperation)
{
    ComponentTransferFunction transferFunction;
    transferFunction.type = FECOMPONENTTRANSFER_TYPE_LINEAR;
    float amount = narrowPrecisionToFloat(componentTransferOperation.amount());
    transferFunction.slope = amount;
    transferFunction.intercept = -0.5 * amount + 0.5;

    ComponentTransferFunction nullFunction;
    return FEComponentTransfer::create(transferFunction, transferFunction, transferFunction, nullFunction);
}

static RefPtr<FilterEffect> createDropShadowEffect(const DropShadowFilterOperation& dropShadowOperation)
{
    float std = dropShadowOperation.stdDeviation();
    return FEDropShadow::create(std, std, dropShadowOperation.x(), dropShadowOperation.y(), dropShadowOperation.color(), 1);
}

static RefPtr<FilterEffect> createGrayScaleEffect(const BasicColorMatrixFilterOperation& colorMatrixOperation)
{
    auto grayscaleMatrix = grayscaleColorMatrix(colorMatrixOperation.amount());
    Vector<float> inputParameters {
        grayscaleMatrix.at(0, 0), grayscaleMatrix.at(0, 1), grayscaleMatrix.at(0, 2), 0, 0,
        grayscaleMatrix.at(1, 0), grayscaleMatrix.at(1, 1), grayscaleMatrix.at(1, 2), 0, 0,
        grayscaleMatrix.at(2, 0), grayscaleMatrix.at(2, 1), grayscaleMatrix.at(2, 2), 0, 0,
        0, 0, 0, 1, 0,
    };

    return FEColorMatrix::create(FECOLORMATRIX_TYPE_MATRIX, WTFMove(inputParameters));
}

static RefPtr<FilterEffect> createHueRotateEffect(const BasicColorMatrixFilterOperation& colorMatrixOperation)
{
    Vector<float> inputParameters { narrowPrecisionToFloat(colorMatrixOperation.amount()) };
    return FEColorMatrix::create(FECOLORMATRIX_TYPE_HUEROTATE, WTFMove(inputParameters));
}

static RefPtr<FilterEffect> createInvertEffect(const BasicComponentTransferFilterOperation& componentTransferOperation)
{
    ComponentTransferFunction transferFunction;
    transferFunction.type = FECOMPONENTTRANSFER_TYPE_LINEAR;
    float amount = narrowPrecisionToFloat(componentTransferOperation.amount());
    transferFunction.slope = 1 - 2 * amount;
    transferFunction.intercept = amount;

    ComponentTransferFunction nullFunction;
    return FEComponentTransfer::create(transferFunction, transferFunction, transferFunction, nullFunction);
}

static RefPtr<FilterEffect> createOpacityEffect(const BasicComponentTransferFilterOperation& componentTransferOperation)
{
    ComponentTransferFunction transferFunction;
    transferFunction.type = FECOMPONENTTRANSFER_TYPE_LINEAR;
    float amount = narrowPrecisionToFloat(componentTransferOperation.amount());
    transferFunction.slope = amount;
    transferFunction.intercept = 0;

    ComponentTransferFunction nullFunction;
    return FEComponentTransfer::create(nullFunction, nullFunction, nullFunction, transferFunction);
}

static RefPtr<FilterEffect> createSaturateEffect(const BasicColorMatrixFilterOperation& colorMatrixOperation)
{
    Vector<float> inputParameters { narrowPrecisionToFloat(colorMatrixOperation.amount()) };
    return FEColorMatrix::create(FECOLORMATRIX_TYPE_SATURATE, WTFMove(inputParameters));
}

static RefPtr<FilterEffect> createSepiaEffect(const BasicColorMatrixFilterOperation& colorMatrixOperation)
{
    auto sepiaMatrix = sepiaColorMatrix(colorMatrixOperation.amount());
    Vector<float> inputParameters {
        sepiaMatrix.at(0, 0), sepiaMatrix.at(0, 1), sepiaMatrix.at(0, 2), 0, 0,
        sepiaMatrix.at(1, 0), sepiaMatrix.at(1, 1), sepiaMatrix.at(1, 2), 0, 0,
        sepiaMatrix.at(2, 0), sepiaMatrix.at(2, 1), sepiaMatrix.at(2, 2), 0, 0,
        0, 0, 0, 1, 0,
    };

    return FEColorMatrix::create(FECOLORMATRIX_TYPE_MATRIX, WTFMove(inputParameters));
}

bool FilterUltralight::buildFilterFunctions(const FilterOperations& operations, OptionSet<FilterRenderingMode> preferredFilterRenderingModes, const FloatRect& targetBoundingBox, const GraphicsContext& destinationContext)
{
    RefPtr<FilterFunction> function;

    for (auto& operation : operations.operations()) {
        switch (operation->type()) {
        case FilterOperation::Type::AppleInvertLightness:
            ASSERT_NOT_REACHED(); // AppleInvertLightness is only used in -apple-color-filter.
            break;

        case FilterOperation::Type::Blur:
            function = createBlurEffect(downcast<BlurFilterOperation>(*operation), clipOperation());
            break;

        case FilterOperation::Type::Brightness:
            function = createBrightnessEffect(downcast<BasicComponentTransferFilterOperation>(*operation));
            break;

        case FilterOperation::Type::Contrast:
            function = createContrastEffect(downcast<BasicComponentTransferFilterOperation>(*operation));
            break;

        case FilterOperation::Type::DropShadow:
            function = createDropShadowEffect(downcast<DropShadowFilterOperation>(*operation));
            break;

        case FilterOperation::Type::Grayscale:
            function = createGrayScaleEffect(downcast<BasicColorMatrixFilterOperation>(*operation));
            break;

        case FilterOperation::Type::HueRotate:
            function = createHueRotateEffect(downcast<BasicColorMatrixFilterOperation>(*operation));
            break;

        case FilterOperation::Type::Invert:
            function = createInvertEffect(downcast<BasicComponentTransferFilterOperation>(*operation));
            break;

        case FilterOperation::Type::Opacity:
            function = createOpacityEffect(downcast<BasicComponentTransferFilterOperation>(*operation));
            break;

        case FilterOperation::Type::Saturate:
            function = createSaturateEffect(downcast<BasicColorMatrixFilterOperation>(*operation));
            break;

        case FilterOperation::Type::Sepia:
            function = createSepiaEffect(downcast<BasicColorMatrixFilterOperation>(*operation));
            break;

        default:
            break;
        }

        if (!function)
            continue;

        if (m_functions.isEmpty())
            m_functions.append(SourceGraphic::create());

        m_functions.append(function.releaseNonNull());
    }

    // If we didn't make any effects, tell our caller we are not valid.
    if (m_functions.isEmpty())
        return false;

    m_functions.shrinkToFit();
    return true;
}

FilterEffectVector FilterUltralight::effectsOfType(FilterFunction::Type filterType) const
{
    FilterEffectVector effects;

    for (auto& function : m_functions) {
        if (function->filterType() == filterType) {
            effects.append({ downcast<FilterEffect>(function.get()) });
            continue;
        }

        if (function->isSVGFilter()) {
            auto& filter = downcast<SVGFilter>(function.get());
            effects.appendVector(filter.effectsOfType(filterType));
        }
    }

    return effects;
}

OptionSet<FilterRenderingMode> FilterUltralight::supportedFilterRenderingModes() const
{
    OptionSet<FilterRenderingMode> modes = allFilterRenderingModes;

    for (auto& function : m_functions)
        modes = modes & function->supportedFilterRenderingModes();

    ASSERT(modes);
    return modes;
}

RefPtr<FilterImage> FilterUltralight::apply(FilterImage* sourceImage, FilterResults& results)
{
    ProfiledZone;
    if (!sourceImage)
        return nullptr;
    
    RefPtr<FilterImage> result = sourceImage;

    for (auto& function : m_functions) {
        result = function->apply(*this, *result, results);
        if (!result)
            return nullptr;
    }

    return result;
}

FilterStyleVector FilterUltralight::createFilterStyles(const FilterStyle& sourceStyle) const
{
    ASSERT(supportedFilterRenderingModes().contains(FilterRenderingMode::GraphicsContext));

    FilterStyleVector styles;
    FilterStyle lastStyle = sourceStyle;

    for (auto& function : m_functions) {
        if (function->filterType() == FilterEffect::Type::SourceGraphic)
            continue;

        auto result = function->createFilterStyles(*this, lastStyle);
        if (result.isEmpty())
            return { };

        lastStyle = result.last();
        styles.appendVector(WTFMove(result));
    }

    return styles;
}

void FilterUltralight::setFilterRegion(const FloatRect& filterRegion)
{
    Filter::setFilterRegion(filterRegion);
    clampFilterRegionIfNeeded();
}

TextStream& FilterUltralight::externalRepresentation(TextStream& ts, FilterRepresentation representation) const
{
    unsigned level = 0;

    for (auto it = m_functions.rbegin(), end = m_functions.rend(); it != end; ++it) {
        auto& function = *it;
        
        // SourceAlpha is a built-in effect. No need to say SourceGraphic is its input.
        if (function->filterType() == FilterEffect::Type::SourceAlpha)
            ++it;

        TextStream::IndentScope indentScope(ts, level++);
        function->externalRepresentation(ts, representation);
    }

    return ts;
}

} // namespace WebCore
