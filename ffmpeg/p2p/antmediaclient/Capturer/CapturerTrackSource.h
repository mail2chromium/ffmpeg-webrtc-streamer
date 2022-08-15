//
// Created by zaid.ali on 11/25/2021.
//

#ifndef SENDVIDEOTOWEB_CAPTURERTRACKSOURCE_H
#define SENDVIDEOTOWEB_CAPTURERTRACKSOURCE_H

#include "vcm_capturer.h"
#include "rtc_base/logging.h"

class CapturerTrackSource : public webrtc::VideoTrackSource {
public:
    static rtc::scoped_refptr<CapturerTrackSource> Create() {
        const size_t kWidth = 1280;//640;
        const size_t kHeight = 720;//480;
        const size_t kFps = 60;
        const size_t kDeviceIndex = 0;
        std::unique_ptr<webrtc::test::VcmCapturer> capturer = absl::WrapUnique(
                webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, kDeviceIndex));
        if (!capturer) {
            return nullptr;
        }
        RTC_LOG(LS_NONE) << "Capturer: " << capturer.get();
        return new rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));
    }

    rtc::VideoSourceInterface<webrtc::VideoFrame> *source() override {
        if (capturer_ != NULL) {
            RTC_LOG(LS_NONE) << "\nCapture_ has data..: " << capturer_.get() << "\n";
        } else {
            RTC_LOG(LS_NONE) << "\nCapture_ is empty\n";
        }
        return capturer_.get();
    }

protected:
    explicit CapturerTrackSource(
            std::unique_ptr<webrtc::test::VcmCapturer> capturer)
            : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {

        if (capturer_ != NULL) {
            RTC_LOG(LS_NONE) << "\nCapture_ is init..: " << capturer_.get() << "\n";
        } else {
            RTC_LOG(LS_NONE) << "\nCapture_ is empty\n";
        }
    }

private:
    std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
};


#endif //SENDVIDEOTOWEB_CAPTURERTRACKSOURCE_H
