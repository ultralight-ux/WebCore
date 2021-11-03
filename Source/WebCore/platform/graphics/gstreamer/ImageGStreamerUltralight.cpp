#include "config.h"
#include "ImageGStreamer.h"

#if ENABLE(VIDEO) && USE(GSTREAMER)

#include "GStreamerCommon.h"

#include <Ultralight/Bitmap.h>
#include <Ultralight/private/VideoFrame.h>
#include <gst/gst.h>
#include <gst/video/gstvideometa.h>

namespace WebCore {

static void OnDestroyVideoFrame()
{
}

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

    GstVideoFrame* gstFrame = new GstVideoFrame;

    bool frameMapped = gst_video_frame_map(gstFrame, &videoInfo, buffer, GST_MAP_READ);
    if (!frameMapped) {
        delete gstFrame;
        return;
    }

    unsigned char* bufferData = reinterpret_cast<unsigned char*>(GST_VIDEO_FRAME_PLANE_DATA(gstFrame, 0));
    int stride = GST_VIDEO_FRAME_PLANE_STRIDE(gstFrame, 0);
    int width = GST_VIDEO_FRAME_WIDTH(gstFrame);
    int height = GST_VIDEO_FRAME_HEIGHT(gstFrame);

    bool has_alpha = false;
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
    has_alpha = (GST_VIDEO_FRAME_FORMAT(gstFrame) == GST_VIDEO_FORMAT_BGRA) ? true : false;
#else
    has_alpha = (GST_VIDEO_FRAME_FORMAT(gstFrame) == GST_VIDEO_FORMAT_ARGB) ? true : false;
#endif

    ultralight::RefPtr<ultralight::Bitmap> ul_bitmap = ultralight::Bitmap::Create(width, height,
        ultralight::BitmapFormat::BGRA8_UNORM_SRGB, stride, bufferData, stride * height, false);

    m_videoFrame = ultralight::VideoFrame::Create(ul_bitmap, [=] {
        gst_video_frame_unmap(gstFrame);
        delete gstFrame;
    });

    /*
    // TODO: Handle crop.
    if (GstVideoCropMeta* cropMeta = gst_buffer_get_video_crop_meta(buffer))
        setCropRect(FloatRect(cropMeta->x, cropMeta->y, cropMeta->width, cropMeta->height));
    */
}

ImageGStreamer::~ImageGStreamer()
{
}

} // namespace WebCore

#endif // USE(GSTREAMER)
