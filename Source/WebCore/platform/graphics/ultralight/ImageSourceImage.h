///
/// Copyright (C) 2024 Ultralight, Inc.
///

#pragma once

#include "Image.h"
#include "NativeImage.h"
#include <Ultralight/ImageSource.h>
#include <Ultralight/private/Image.h>

namespace WebCore {

class ImageSourceImage : public Image, public ultralight::ImageSourceProviderListener, public ultralight::ImageSourceListener {
public:
    static Ref<ImageSourceImage> create(ImageObserver* observer)
    {
        return adoptRef(*new ImageSourceImage(observer));
    }

    inline static bool matchesResource(const String& mimeType, const URL& url)
    {
        return mimeType == "image/imgsrc"_s || url.path().endsWithIgnoringASCIICase(".imgsrc"_s);
    }

    virtual ~ImageSourceImage();

    virtual bool isImageSourceImage() const override { return true; }

    virtual bool currentFrameKnownToBeOpaque() const override { return false; }
    virtual bool isAnimated() const override { return false; }

    virtual FloatSize size(ImageOrientation = ImageOrientation::FromImage) const override;

    virtual EncodedDataStatus dataChanged(bool allDataReceived) override;

    virtual void destroyDecodedData(bool destroyAll = true) override;

    virtual RefPtr<NativeImage> nativeImage(const DestinationColorSpace& = DestinationColorSpace::SRGB()) override;

    virtual ImageDrawResult draw(GraphicsContext&, const FloatRect& dstRect, const FloatRect& srcRect, const ImagePaintingOptions& = {}) override;

protected:
    ImageSourceImage(ImageObserver*);

    // ImageSourceProviderListener
    virtual void OnAddImageSource(const ultralight::String& id, ultralight::RefPtr<ultralight::ImageSource> image_source) override;
    virtual void OnRemoveImageSource(const ultralight::String& id) override;

    // ImageSourceListener
    virtual void OnInvalidateImageSource(ultralight::ImageSource* image_source) override;

    std::optional<ultralight::String> decodeImageIdentifier(FragmentedSharedBuffer& data);

    // Clears the image source and detaches any listeners.
    void clearImageSource();

    // Attempt to create the image source using the image identifier.
    void updateImageSource();

    EncodedDataStatus m_encodedDataStatus { EncodedDataStatus::TypeAvailable };
    DecodingStatus m_decodingStatus { DecodingStatus::Decoding };
    bool m_hasImageIdentifier { false };
    ultralight::String m_imageIdentifier;
    ultralight::RefPtr<ultralight::ImageSource> m_imageSource;
    RefPtr<NativeImage> m_nativeImage;
};

} // namespace WebCore