//
// Created by Ottoman on 20/11/2021.
//

#ifndef QUICKRTC_VCM_CAPTURER_H
#define QUICKRTC_VCM_CAPTURER_H

#include <iostream>
#include <string>
using namespace std;

/// methods to export video packets
extern "C" void SendVideoPackets(int);
extern "C" void InitializeVideoPackets(int);
extern "C" void FillVideoPackets(int, uint8_t);
extern "C" void DeleteVideoPackets();
extern "C" void SetPreEncoding4Video();
extern "C" void SetPTS4Video(int64_t);
extern "C" int WriteAPI4Video(uint8_t* data, int size);
static int VideoCodecIsRequested;

/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_VCM_CAPTURER_H_
#define TEST_VCM_CAPTURER_H_

#include <memory>
#include <vector>

#include "api/scoped_refptr.h"
#include "modules/video_capture/video_capture.h"
#include "test/test_video_capturer.h"

namespace webrtc {
    namespace test {

        class VcmCapturer : public TestVideoCapturer,
                            public rtc::VideoSinkInterface<VideoFrame> {
        public:
            static VcmCapturer* Create(size_t width,
                                       size_t height,
                                       size_t target_fps,
                                       size_t capture_device_index);
            ~VcmCapturer() override;

            void OnFrame(const VideoFrame& frame) override;

        private:
            VcmCapturer();
            bool Init(size_t width,
                      size_t height,
                      size_t target_fps,
                      size_t capture_device_index);
            void Destroy();

            rtc::scoped_refptr<VideoCaptureModule> vcm_;
            VideoCaptureCapability capability_;
        };

    }  // namespace test
}  // namespace webrtc

#endif  // TEST_VCM_CAPTURER_H_


#endif //QUICKRTC_VCM_CAPTURER_H
