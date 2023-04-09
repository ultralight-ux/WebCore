#include "config.h"
#include "ImageBufferUtilitiesUltralight.h"

#if USE(ULTRALIGHT)

namespace WebCore {

static bool encodeImage(ultralight::Image* image, const String& mimeType, Vector<uint8_t>* output)
{
    ASSERT_UNUSED(mimeType, mimeType == "image/png"_s); // Only PNG output is supported for now.
    if (mimeType != "image/png"_s)
        return false;

    ultralight::RefPtr<ultralight::Buffer> buffer = image->bitmap()->EncodePNG();
    if (!buffer)
        return false;

    return output->tryAppend((uint8_t*)buffer->data(), buffer->size());
}

Vector<uint8_t> encodeData(ultralight::Image* image, const String& mimeType, std::optional<double>)
{
    Vector<uint8_t> encodedImage;
    if (!image || !encodeImage(image, mimeType, &encodedImage))
        return {};
    return encodedImage;
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)