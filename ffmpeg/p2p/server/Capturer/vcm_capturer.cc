//
// Created by Ottoman on 20/11/2021.
//

#include "vcm_capturer.h"
//#include "test/win/d3d_renderer.h"
#include "../WebSocket/WebSocketHelper.h"
#include "video/video_send_stream.h"

/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdint.h>
#include <vector>
#include <memory>
#include "H264FrameBuffer.h"


bool oneTime = true;
namespace webrtc {
    namespace test {

        int time = 0;


        //to render video localy
//        webrtc::test::D3dRenderer *video_renderer;

//        void renderer() {
//            RTC_LOG(LS_NONE) << "Renderer Thread started\n";
//            video_renderer
//                    = webrtc::test::D3dRenderer::Create("Output", 640, 480);
//        }

        VcmCapturer::VcmCapturer() : vcm_(nullptr) {

        }

        bool VcmCapturer::Init(size_t width,
                               size_t height,
                               size_t target_fps,
                               size_t capture_device_index) {
            std::unique_ptr<VideoCaptureModule::DeviceInfo> device_info(
                    VideoCaptureFactory::CreateDeviceInfo());

            char device_name[256];
            char unique_name[256];
            if (device_info->GetDeviceName(static_cast<uint32_t>(capture_device_index),
                                           device_name, sizeof(device_name), unique_name,
                                           sizeof(unique_name)) != 0) {
                Destroy();
                return false;
            }

            vcm_ = webrtc::VideoCaptureFactory::Create(unique_name);
            if (!vcm_) {
                return false;
            }
            vcm_->RegisterCaptureDataCallback(this);

            device_info->GetCapability(vcm_->CurrentDeviceName(), 0, capability_);

            capability_.width = static_cast<int32_t>(width);
            capability_.height = static_cast<int32_t>(height);
            capability_.maxFPS = static_cast<int32_t>(target_fps);
            capability_.videoType = VideoType::kI420;

            if (vcm_->StartCapture(capability_) != 0) {
                Destroy();
                return false;
            }

            RTC_CHECK(vcm_->CaptureStarted());

            return true;
        }

        VcmCapturer *VcmCapturer::Create(size_t width,
                                         size_t height,
                                         size_t target_fps,
                                         size_t capture_device_index) {
            std::unique_ptr<VcmCapturer> vcm_capturer(new VcmCapturer());
//            if (!vcm_capturer->Init(width, height, target_fps, capture_device_index)) {
//                RTC_LOG(LS_WARNING) << "Failed to create VcmCapturer(w = " << width
//                                    << ", h = " << height << ", fps = " << target_fps
//                                    << ")";
//                return nullptr;
//            }
            RTC_LOG(LS_NONE) << "\nVC Capturer my class\n";
            return vcm_capturer.release(); //error here
        }

        void VcmCapturer::Destroy() {
            if (!vcm_)
                return;

            vcm_->StopCapture();
            vcm_->DeRegisterCaptureDataCallback();
            // Release reference to VCM.
            vcm_ = nullptr;
        }

        VcmCapturer::~VcmCapturer() {
            Destroy();
        }

        void VcmCapturer::OnFrame(const VideoFrame &frame) {

            RTC_LOG(LS_NONE) << "------------OnFrame vcm capturer----------------\n";
            RTC_LOG(LS_NONE) << "Video Stream Encoder Extern: " << webrtc::video_stream_encoder_extern.get();

            if (webrtc::video_stream_encoder_extern) {
                if (oneTime) {
                    webrtc::test::TestVideoCapturer::AddOrUpdateSink(webrtc::video_stream_encoder_extern.get(),
                                                                     rtc::VideoSinkWants());
                    oneTime = false;
                }

                if (!oneTime) {
                    webrtc::test::TestVideoCapturer::OnFrame(frame);
                }
            }

            RTC_LOG(LS_NONE) << "Frames: " << ++time;

        }

    }  // namespace test
}  // namespace webrtc
rtc::VideoBroadcaster broadcaster_;


std::vector<uint8_t> import_buff;

void send_to_vc_wrapper(int pkt_size) {
    /*TODO import_buff is ready to dispatch over VideoChannel*/
    RTC_LOG(LS_NONE) << pkt_size << ": Inside Callback Function";
    int Width = 640, Height = 480; //640x480
//    rtc::scoped_refptr<FH264FrameBuffer> buffer = new rtc::RefCountedObject<FH264FrameBuffer>(Width, Height);
//    webrtc::VideoFrame Frame{buffer, webrtc::VideoRotation::kVideoRotation_0, 0};
//
//    RTC_LOG(LS_ERROR) << "pktframetype is:  " << Frame.video_frame_buffer()->type();
//
//    // #Andriy: WebRTC doesn't like frames with the same timestamp and will drop one of them
//    // we don't like our frames to be dropped so let's cheat with setting a unique value but close to be true
//    int64_t NtpTimeMs = rtc::TimeMillis();
////    if (NtpTimeMs <= LastNtpTimeMs)
////        NtpTimeMs = LastNtpTimeMs + 1;
////    LastNtpTimeMs = NtpTimeMs;
//    Frame.set_ntp_time_ms(NtpTimeMs);
//
//    auto PkData = reinterpret_cast<const uint8_t *>(import_buff.data());
//
//    buffer->GetBuffer().assign(PkData, PkData + pkt_size);
//
////     webrtc::VideoFrame Frame = webrtc::VideoFrame::Builder()
////                                    .set_video_frame_buffer(buffer)
////                                    .set_ntp_time_ms(NtpTimeMs)
////                                    .set_rotation(webrtc::VideoRotation::kVideoRotation_0)
////                                    .build();
    int stride_y = Width;
    int stride_uv = (Width + 1) / 2;
    libyuv::RotationMode rotation_mode = libyuv::kRotate0;
    rtc::scoped_refptr<webrtc::I420Buffer> buffer = webrtc::I420Buffer::Create(
            Width, Height, stride_y, stride_uv, stride_uv);

    const int conversionResult = libyuv::ConvertToI420(
            import_buff.data(), pkt_size, buffer.get()->MutableDataY(),
            buffer.get()->StrideY(), buffer.get()->MutableDataU(),
            buffer.get()->StrideU(), buffer.get()->MutableDataV(),
            buffer.get()->StrideV(), 0, 0,  // No Cropping
            Width, Height, Width, Height, rotation_mode,
            ConvertVideoType(webrtc::VideoType::kI420));
    if (conversionResult < 0) {
        RTC_LOG(LS_ERROR) << "Failed to convert capture frame from type "
                          << webrtc::VideoType::kI420 << "to I420.";
    }

    webrtc::VideoFrame captureFrame =
            webrtc::VideoFrame::Builder()
                    .set_video_frame_buffer(buffer)
                    .set_timestamp_rtp(0)
                    .set_timestamp_ms(rtc::TimeMillis())
                    .set_rotation(webrtc::kVideoRotation_0)
                    .build();

    if (webrtc::video_stream_encoder_extern) {
        if (oneTime) {
            RTC_LOG(LS_NONE) << pkt_size << ": Inside AddorUpdateSink Callback Function";
            broadcaster_.AddOrUpdateSink(webrtc::video_stream_encoder_extern.get(),
                                         rtc::VideoSinkWants());

            oneTime = false;
        }

        if (!oneTime) {
            RTC_LOG(LS_NONE) << pkt_size << ": Inside Broadcasting Frame Callback Function";
            broadcaster_.OnFrame(captureFrame);
        }
    }
//

}

void init(int) {}

void fill(int pos, uint8_t data) {
    import_buff.push_back(data);
}

void delete_vc_import() {
    import_buff.clear();
}