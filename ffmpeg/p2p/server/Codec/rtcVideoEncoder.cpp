
// Copyright Ottoman 2022, Inc. All Rights Reserved.

#include <api/video_codecs/sdp_video_format.h>
#include <common_types.h>
#include <h264_profile_level_id.h>
#include <media_constants.h>
#include <api/video_codecs/video_encoder_factory.h>
#include <api/video/video_codec_type.h>
#include <modules/video_coding/codecs/h264/include/h264_globals.h>
#include <api/video_codecs/video_codec.h>
#include <api/video/video_frame.h>
#include <api/video/video_frame_type.h>
#include <common_video/h264/h264_common.h>
#include <rtc_base/ref_counted_object.h>
#include <common_video/libyuv/include/webrtc_libyuv.h>
#include <libyuv/convert.h>
#include <fstream>
#include <api/video/i420_buffer.h>
#include "rtcVideoEncoder.h"

#include "rtc_base/logging.h"

#include "H264FrameBuffer.h"

static int WriteYUV(const char *filename, uint32_t size, uint8_t *YUV) {
    FILE *fout = fopen(filename, "wb");

    size_t result_out = fwrite(YUV, sizeof(unsigned char), size, fout);
    if (result_out != size) {
        perror("Error writing file");
        fclose(fout);
        return 3;
    }

    fclose(fout);
    return 0;
}

namespace {
    webrtc::SdpVideoFormat CreateH264Format(webrtc::H264::Profile profile, webrtc::H264::Level level) {
        const absl::optional<std::string> profile_string =
                webrtc::H264::ProfileLevelIdToString(webrtc::H264::ProfileLevelId(profile, level));
//        RTC_LOG(LS_NONE)<< cricket::kH264FmtpProfileLevelId<<"  "<<*profile_string;
//		check(profile_string);
        return webrtc::SdpVideoFormat(
                cricket::kH264CodecName,
                {{cricket::kH264FmtpProfileLevelId,        *profile_string},
                 {cricket::kH264FmtpLevelAsymmetryAllowed, "1"},
                 {cricket::kH264FmtpPacketizationMode,     "1"}});
    }
}

//////////////////////////////////////////////////////////////////////////
//
// FVideoEncoderFactory
//
//////////////////////////////////////////////////////////////////////////

FVideoEncoderFactory::FVideoEncoderFactory(IVideoEncoderObserver &VideoSource)
        : VideoSource(&VideoSource) {
}


std::vector<webrtc::SdpVideoFormat> FVideoEncoderFactory::GetSupportedFormats() const {

    return {
//            CreateH264Format(webrtc::H264::kProfileHigh, webrtc::H264::kLevel5_1),
//            CreateH264Format(webrtc::H264::kProfileMain, webrtc::H264::kLevel3_1),
//            CreateH264Format(webrtc::H264::kProfileBaseline, webrtc::H264::kLevel3_1),
//            CreateH264Format(webrtc::H264::kProfileConstrainedBaseline, webrtc::H264::kLevel3_1),
            CreateH264Format(webrtc::H264::kProfileConstrainedBaseline, webrtc::H264::kLevel5_1)

    };
}

webrtc::VideoEncoderFactory::CodecInfo
FVideoEncoderFactory::QueryVideoEncoder(const webrtc::SdpVideoFormat &Format) const {
    CodecInfo Info;
    Info.is_hardware_accelerated = true;
    Info.has_internal_source = false;
    return Info;
}

std::unique_ptr<webrtc::VideoEncoder> FVideoEncoderFactory::CreateVideoEncoder(const webrtc::SdpVideoFormat &Format) {
//	FClientSession* Session;
//	bool res = PendingClientSessions.Pop(Session, 0);
//	checkf(res, TEXT("no client session associated with encoder instance"   ));

    auto VideoEncoder = std::make_unique<CustomVideoEncoder>(*VideoSource);
    return VideoEncoder;
}

//
// CustomVideoEncoder
//

CustomVideoEncoder::CustomVideoEncoder(IVideoEncoderObserver &Observer)
        : Observer(&Observer) {
    CodecSpecific.codecType = webrtc::kVideoCodecH264;
    CodecSpecific.codecSpecific.H264.packetization_mode = webrtc::H264PacketizationMode::NonInterleaved;
}

void CustomVideoEncoder::SetQualityControlOwnership(bool bOwnership) {

}

bool CustomVideoEncoder::HasQualityControlOwnership() {
    return true;
}

void CustomVideoEncoder::SetFecControllerOverride(webrtc::FecControllerOverride *fec_controller_override) {
}

webrtc::VideoEncoder::ScalingSettings CustomVideoEncoder::GetScalingSettings() const {
    return ScalingSettings{0, 1024 * 1024};
}

int32_t
CustomVideoEncoder::InitEncode(const webrtc::VideoCodec *codecSettings, const webrtc::VideoEncoder::Settings &settings) {
    EncodedImage._completeFrame = true;
    return 0;
}

int32_t CustomVideoEncoder::RegisterEncodeCompleteCallback(webrtc::EncodedImageCallback *InCallback) {
    this->Callback = InCallback;
    return 0;
}

int32_t CustomVideoEncoder::Release() {
    Callback = nullptr;
    return 0;
}

int32_t CustomVideoEncoder::Encode(
        const webrtc::VideoFrame &Frame,
        const std::vector<webrtc::VideoFrameType> *FrameTypes) {

    H264FrameBuffer *H264Frame = static_cast<H264FrameBuffer *>(Frame.video_frame_buffer().get());
    std::vector<uint8_t> const &FrameBuffer = H264Frame->GetBuffer();

    EncodedImage._encodedWidth = Frame.video_frame_buffer()->width();
    EncodedImage._encodedHeight = Frame.video_frame_buffer()->height();
    // Updated by Muhammad Usman
    EncodedImage.SetTimestamp(Frame.timestamp());
    EncodedImage.ntp_time_ms_ = Frame.ntp_time_ms();
    EncodedImage.capture_time_ms_ = Frame.render_time_ms();
    EncodedImage.rotation_ = Frame.rotation();
    EncodedImage.content_type_ = webrtc::VideoContentType::UNSPECIFIED;
    EncodedImage.timing_.flags = webrtc::VideoSendTiming::TimingFrameFlags::kInvalid;

    EncodedImage._frameType = webrtc::VideoFrameType::kVideoFrameDelta;
    std::vector<webrtc::H264::NaluIndex> NALUIndices =
            webrtc::H264::FindNaluIndices(&FrameBuffer[0], FrameBuffer.size());
    bool bKeyFrameFound = false;
    for (const webrtc::H264::NaluIndex &Index: NALUIndices) {

        webrtc::H264::NaluType NALUType = webrtc::H264::ParseNaluType(FrameBuffer[Index.payload_start_offset]);

        if (NALUType == webrtc::H264::kIdr
            && !bKeyFrameFound) {
            EncodedImage._frameType = webrtc::VideoFrameType::kVideoFrameKey;
            // EG_LOG(LogDefault, Log, "key-frame");
            bKeyFrameFound = true;
            // break; // we need to parse all NALUs so as H264BitstreamParser maintains internal state
            break;
        }
    }

    // enforce key-frame if requested by webrtc and if we haven't received one
    // seems it's always just one FrameType provided, as reference implementation of
    // H264EncoderImpl checks only the first one
//    if (EncodedImage._frameType != webrtc::VideoFrameType::kVideoFrameKey && FrameTypes &&
//        (*FrameTypes)[0] == webrtc::VideoFrameType::kVideoFrameKey)
//    {
////        EG_LOG(LogDefault, Log, "key-frame requested, size=%zu", FrameTypes->size());
//
//        // #MULTICAST : Should we limit what video encoder instances ask for keyframes?
//        if (bOwnsQualityControl)
//            Observer->ForceKeyFrame();
//    }

    // when we switch quality control to client with higher B/W WebRTC won't notify us that bitrate can
    // be increased. So force set last recorded bitrate for this client though we also could set just sufficiently
    // big number to force webRTC to report what actual B/W is

    if (LastBitrate.get_sum_kbps() > 0) {
        // Updated by Muhammad Usman
        // Replacement of SetRateAllocation();
//        rate_control_parameters.bitrate = LastBitrate;
//        rate_control_parameters.framerate_fps = LastFramerate;
        rate_control_parameters.bitrate = LastBitrate;
        rate_control_parameters.framerate_fps = LastFramerate;

        SetRates(rate_control_parameters);
    }

    // BitstreamParser.GetLastSliceQp(&EncodedImage.qp_);

    // copy frame buffer
    // EncodedImageBuffer.resize(FrameBuffer.size());
    EncodedImageBuffer.assign(begin(FrameBuffer), end(FrameBuffer));

    // Updated by Muhammad Usman
    // Replacement 	EncodedImage._buffer = &EncodedImageBuffer[0];
    // EncodedImage._length = EncodedImage._size = EncodedImageBuffer.size();
    /*
    webrtc::EncodedImageBuffer *encoded_image_data_ = webrtc::EncodedImageBuffer::Create(
        reinterpret_cast<const uint8_t *>(EncodedImageBuffer[0]), EncodedImageBuffer.size());*/

    rtc::scoped_refptr<webrtc::EncodedImageBufferInterface> encoded_data_;

    encoded_data_ = webrtc::EncodedImageBuffer::Create(reinterpret_cast<const uint8_t *>(&EncodedImageBuffer[0]),
                                                       EncodedImageBuffer.size());

    EncodedImage.SetEncodedData(encoded_data_.get());

    //EncodedImage.set_size(encoded_image_data_->size());

    // fill RTP fragmentation info
    FragHeader.VerifyAndAllocateFragmentationHeader(NALUIndices.size());
    FragHeader.fragmentationVectorSize = static_cast<uint16_t>(NALUIndices.size());
    for (int I = 0; I != NALUIndices.size(); ++I) {
        webrtc::H264::NaluIndex const &NALUIndex = NALUIndices[I];
        FragHeader.fragmentationOffset[I] = NALUIndex.payload_start_offset;
        FragHeader.fragmentationLength[I] = NALUIndex.payload_size;

        webrtc::H264::NaluType NALUType = webrtc::H264::ParseNaluType(FrameBuffer[NALUIndex.payload_start_offset]);
#if 0
        EG_LOG(
            LogDefault,
            Log,
            "NALU: %d, start=%z, payload=%z",
            static_cast<int>(NALUType),
            NALUIndex.start_offset,
            NALUIndex.payload_size);
#endif
    }

    // Deliver encoded image.
    Callback->OnEncodedImage(EncodedImage, &CodecSpecific, &FragHeader);

    ++FrameNo;

    return 0;
}


void CustomVideoEncoder::OnRttUpdate(int64_t rtt_ms) {
    RTC_LOG(LS_ERROR)
    << "RTT: " << rtt_ms;
}

void CustomVideoEncoder::OnPacketLossRateUpdate(float packet_loss_rate) {
    RTC_LOG(LS_ERROR)
    << "Packet Loss: " << packet_loss_rate;
}

void CustomVideoEncoder::SetRates(const RateControlParameters &parameters) {

    // it seems webrtc just reports the current framerate w/o much effort to probe what's
    // max framerate it can achieve
    // let's lift it a bit every time so we can keep it as high as possible
    uint32_t LiftedFramerate =
            parameters.framerate_fps + std::min(static_cast<uint32_t>(parameters.framerate_fps * 0.9), 1u);

    RTC_LOG(LS_ERROR)
    << "Parameters bitrate: " << parameters.bitrate.get_sum_kbps() << ", fps: " << parameters.framerate_fps;
    Observer->SetRate(512000, 25);

    return;
}

webrtc::VideoEncoder::EncoderInfo CustomVideoEncoder::GetEncoderInfo() const {
    webrtc::VideoEncoder::EncoderInfo encoder_info;

    encoder_info.scaling_settings = ScalingSettings{0, 1024 * 1024};
    encoder_info.supports_native_handle = true;
    encoder_info.is_hardware_accelerated = true;
    encoder_info.has_internal_source = false;

    return encoder_info;
}

bool CustomVideoEncoder::SupportsNativeHandle() const {
    webrtc::VideoEncoder::EncoderInfo encoder_info;
    encoder_info.supports_native_handle = true;

    return true;
}