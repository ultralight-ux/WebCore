#include "config.h"
#include "ImageGStreamer.h"

#if ENABLE(VIDEO) && USE(GSTREAMER)

#include "GStreamerCommon.h"

#include <gst/gst.h>
#include <gst/video/gstvideometa.h>
#include <Ultralight/Bitmap.h>
#include <Ultralight/private/Image.h>

namespace WebCore {

ImageGStreamer::ImageGStreamer(GstSample* sample)
{
    GstCaps* caps = gst_sample_get_caps(sample);
    GstVideoInfo videoInfo;
    gst_video_info_init(&videoInfo);
    if (!gst_video_info_from_caps(&videoInfo, caps))
        return;

    // Right now the TextureMapper only supports chromas with one plane
    ASSERT(GST_VIDEO_INFO_N_PLANES(&videoInfo) == 1);

    GstBuffer* buffer = gst_sample_get_buffer(sample);
    if (UNLIKELY(!GST_IS_BUFFER(buffer)))
        return;

    m_frameMapped = gst_video_frame_map(&m_videoFrame, &videoInfo, buffer, GST_MAP_READ);
    if (!m_frameMapped)
        return;

    unsigned char* bufferData = reinterpret_cast<unsigned char*>(GST_VIDEO_FRAME_PLANE_DATA(&m_videoFrame, 0));
    int stride = GST_VIDEO_FRAME_PLANE_STRIDE(&m_videoFrame, 0);
    int width = GST_VIDEO_FRAME_WIDTH(&m_videoFrame);
    int height = GST_VIDEO_FRAME_HEIGHT(&m_videoFrame);

    ultralight::RefPtr<ultralight::Bitmap> ul_bitmap;
    ultralight::RefPtr<ultralight::Image> ul_image = ultralight::Image::Create();

    bool has_alpha = false;
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
    has_alpha = (GST_VIDEO_FRAME_FORMAT(&m_videoFrame) == GST_VIDEO_FORMAT_BGRA) ? true : false;
#else
    has_alpha = (GST_VIDEO_FRAME_FORMAT(&m_videoFrame) == GST_VIDEO_FORMAT_ARGB) ? true : false;
#endif

    // GStreamer doesn't use premultiplied alpha, but Ultralight does. So if the video format has an alpha component
    // we need to premultiply it before passing the data to Ultralight. This needs to be both using gstreamer-gl and not
    // using it.
    //
    // This method could be called several times for the same buffer, for example if we are rendering the video frames
    // in several non accelerated canvases. Due to this, we cannot modify the buffer, so we need to create a copy.
    if (has_alpha) {
        ul_bitmap = ultralight::Bitmap::Create(width, height,
            ultralight::kBitmapFormat_BGRA8_UNORM_SRGB);

        unsigned char* surfaceData = (unsigned char*)ul_bitmap->LockPixels();
        unsigned char* surfacePixel = surfaceData;

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
                // Video frames use BGRA in little endian.
                unsigned short alpha = bufferData[3];
                surfacePixel[0] = (bufferData[0] * alpha + 128) / 255;
                surfacePixel[1] = (bufferData[1] * alpha + 128) / 255;
                surfacePixel[2] = (bufferData[2] * alpha + 128) / 255;
                surfacePixel[3] = alpha;
#else
                // Video frames use ARGB in big endian.
                unsigned short alpha = bufferData[0];
                surfacePixel[0] = alpha;
                surfacePixel[1] = (bufferData[1] * alpha + 128) / 255;
                surfacePixel[2] = (bufferData[2] * alpha + 128) / 255;
                surfacePixel[3] = (bufferData[3] * alpha + 128) / 255;
#endif
                bufferData += 4;
                surfacePixel += 4;
            }
        }

        ul_bitmap->UnlockPixels();
        ul_image->SetFrame(0, 1, *ul_bitmap, false);
    } else {
        // Wrap existing pixels
        ul_bitmap = ultralight::Bitmap::Create(width, height, ultralight::kBitmapFormat_BGRA8_UNORM_SRGB, stride, bufferData, stride * height, false);

        ul_bitmap->UnlockPixels();
        ul_image->SetFrame(0, 1, *ul_bitmap, false);
    }

    NativeImagePtr surface;
    surface.reset(new FramePair(ul_image, 0));
    m_image = BitmapImage::create(WTFMove(surface));

    if (GstVideoCropMeta* cropMeta = gst_buffer_get_video_crop_meta(buffer))
        setCropRect(FloatRect(cropMeta->x, cropMeta->y, cropMeta->width, cropMeta->height));
}

ImageGStreamer::~ImageGStreamer()
{
    if (m_image)
        m_image = nullptr;

    // We keep the buffer memory mapped until the image is destroyed because our
    // ultralight::Bitmap may wrap the internal bitmap bytes.
    if (m_frameMapped)
        gst_video_frame_unmap(&m_videoFrame);
}

} // namespace WebCore

#endif // USE(GSTREAMER)
