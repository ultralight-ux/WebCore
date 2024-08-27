///
/// Copyright (C) 2024 Ultralight, Inc.
///

#include "config.h"
#include "ImageSourceImage.h"
#include "ImageObserver.h"
#include "GraphicsContext.h"
#include "SharedBuffer.h"

namespace WebCore {

ImageSourceImage::ImageSourceImage(ImageObserver* observer)
    : Image(observer)
{
    ultralight::ImageSourceProvider::instance().AddListener(this);
}

ImageSourceImage::~ImageSourceImage()
{
    clearImageSource();
    ultralight::ImageSourceProvider::instance().RemoveListener(this);
}

FloatSize ImageSourceImage::size(ImageOrientation) const
{
    if (m_imageSource)
        return FloatSize(m_imageSource->width(), m_imageSource->height());

    return FloatSize();
}

EncodedDataStatus ImageSourceImage::dataChanged(bool allDataReceived)
{
    if (m_encodedDataStatus == EncodedDataStatus::TypeAvailable && allDataReceived) {
        if (!data() || data()->isEmpty()) {
            m_encodedDataStatus = EncodedDataStatus::Error;
            m_decodingStatus = DecodingStatus::Invalid;
            return m_encodedDataStatus;
        }

        // We've received all data, decode the image identifier now.
        if (auto imageIdentifier = decodeImageIdentifier(*data())) {
            m_imageIdentifier = imageIdentifier.value();
            m_hasImageIdentifier = true;
            m_encodedDataStatus = EncodedDataStatus::Complete;
            m_decodingStatus = DecodingStatus::Complete;

            if (imageObserver())
                imageObserver()->decodedSizeChanged(*this, m_imageIdentifier.utf8().sizeBytes());

            // Attempt to create the image source using the image identifier.
            // This may fail if no image source is available in ImageSourceProvider.
            updateImageSource();
        } else {
            m_encodedDataStatus = EncodedDataStatus::Error;
            m_decodingStatus = DecodingStatus::Invalid;
        }
    }

    return m_encodedDataStatus;
}

void ImageSourceImage::destroyDecodedData(bool destroyAll)
{
}

RefPtr<NativeImage> ImageSourceImage::nativeImage(const DestinationColorSpace&)
{
    return m_nativeImage;
}

ImageDrawResult ImageSourceImage::draw(GraphicsContext& context, const FloatRect& dstRect, const FloatRect& srcRect, const ImagePaintingOptions& options)
{
    if (!m_nativeImage || dstRect.isEmpty() || srcRect.isEmpty())
        return ImageDrawResult::DidNothing;

    context.drawNativeImage(*m_nativeImage, size(), dstRect, srcRect, options);

    if (imageObserver())
        imageObserver()->didDraw(*this);

    return ImageDrawResult::DidDraw;
}

// ImageSourceProviderListener
void ImageSourceImage::OnAddImageSource(const ultralight::String& id, ultralight::RefPtr<ultralight::ImageSource> image_source)
{
    // Ignore if we already have an image source
    if (m_imageSource)
        return;

    // Ignore if the image source id doesn't match
    if (id != m_imageIdentifier)
        return;

    updateImageSource();
}

void ImageSourceImage::OnRemoveImageSource(const ultralight::String& id)
{
    // Ignore if the image source id doesn't match
    if (id != m_imageIdentifier)
        return;

    clearImageSource();
}

// ImageSourceListener
void ImageSourceImage::OnInvalidateImageSource(ultralight::ImageSource* image_source)
{
    if (image_source != m_imageSource)
        return;

    if (imageObserver()) {
        IntRect rect = enclosingIntRect(FloatRect(FloatPoint(), size()));
        imageObserver()->changedInRect(*this, &rect);
    }
}

std::optional<ultralight::String> ImageSourceImage::decodeImageIdentifier(FragmentedSharedBuffer& data)
{
    auto buffer = data.makeContiguous();

    // Copy the contents to a WTF::String
    String dataString(reinterpret_cast<const char*>(buffer->data()), buffer->size());

    // The file string starts with the file identifier followed by a newline, then the image id.
    //
    // Example:
    // IMGSRC-V1
    // my-image-id
    //

    // First find the file identifier
    auto fileIdentifierIndex = dataString.find("IMGSRC-V1"_s);
    if (fileIdentifierIndex == notFound)
        return std::nullopt;

    // Find the next newline after the file identifier
    auto newlineIndex = dataString.find('\n', fileIdentifierIndex);
    if (newlineIndex == notFound)
        return std::nullopt;

    // Extract the image id
    auto imageId = dataString.substring(newlineIndex + 1);

    // Trim any whitespace from the image id string
    imageId = imageId.stripWhiteSpace();

    if (imageId.isEmpty())
        return std::nullopt;

    // Convert to ultralight string
    return ultralight::String(imageId.utf8().data());
}

void ImageSourceImage::clearImageSource()
{
    if (m_imageSource)
        m_imageSource->RemoveListener(this);

    m_imageSource = nullptr;
    m_nativeImage = nullptr;
}

void ImageSourceImage::updateImageSource()
{
    if (m_imageSource)
        return;

    if (!m_hasImageIdentifier)
        return;

    // Lookup the image source by id
    m_imageSource = ultralight::ImageSourceProvider::instance().GetImageSource(m_imageIdentifier.utf8().data());

    if (!m_imageSource)
        return;

    // Add listener to the image source so we can invalidate the image when it changes
    m_imageSource->AddListener(this);

    // Create the native image from the existing image (a unique image was created for the
    // image source upon creation)
    m_nativeImage = NativeImage::create(ultralight::Image::FromImageSource(m_imageSource));

    if (imageObserver())
        imageObserver()->imageFrameAvailable(*this, ImageAnimatingState::No, nullptr, m_decodingStatus);
}

} // namespace WebCore