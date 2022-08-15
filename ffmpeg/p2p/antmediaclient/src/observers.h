#pragma once
#include<iostream>

#include "api/peer_connection_interface.h"
#include "api/rtp_transceiver_interface.h"
#include "rtc_base/ref_count.h"
#include "rtc_base/ref_counter.h"


class PeerConnectionObserver : public webrtc::PeerConnectionObserver {
public:
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState stream) override {
        (void) stream;
    }

    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
        (void) stream;
    }

    void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override {
        (void) transceiver;
    }

    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
        (void) stream;
    }

    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {
        (void) channel;
    }

    void OnRenegotiationNeeded() override {}

    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
        (void) new_state;
    }

    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {
        (void) new_state;
    }

    void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) {}
};

class DataChannelObserver : public webrtc::DataChannelObserver {
public:
    void OnStateChange() override {}

    void OnMessage(const webrtc::DataBuffer &buffer) override {
        (void) buffer;
    }

    void OnBufferedAmountChange(uint64_t previous_amount) override {
        (void) previous_amount;
    }
};



class CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver{
public:
    static CreateSessionDescriptionObserver* Create() {
        return new rtc::RefCountedObject<CreateSessionDescriptionObserver>();
    }
    virtual void OnSuccess(webrtc::SessionDescriptionInterface *desc);// { RTC_LOG(INFO) << __FUNCTION__; };
//    virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
    virtual void OnFailure(webrtc::RTCError error) {
        RTC_LOG(INFO) << __FUNCTION__ << "Error: " << ToString(error.type()) << ": "
                      << error.message();
    }

};



class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
public:
    static SetSessionDescriptionObserver* Create() {
        return new rtc::RefCountedObject<SetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
    virtual void OnFailure(webrtc::RTCError error) {
        RTC_LOG(INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
                      << error.message();
    }
};

//class CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver {
//public:
//
//    void OnSuccess(webrtc::SessionDescriptionInterface *desc) override;
//
//    void OnFailure(webrtc::RTCError error) override;
//
//private:
//    const rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc_;
//};





//class SetSessionDescriptionObserver
//        : public webrtc::SetSessionDescriptionObserver {
//
//
//private:
//    mutable webrtc::webrtc_impl::RefCounter ref_count_{0};
//public:
//    void OnSuccess() override;
//
//    void OnFailure(webrtc::RTCError error) override;
//
//    void AddRef() const { ref_count_.IncRef(); }
//
//    rtc::RefCountReleaseStatus Release() const {
//        const auto status = ref_count_.DecRef();
//        if (status == rtc::RefCountReleaseStatus::kDroppedLastRef) {
//            delete this;
//        }
//        return status;
//    }
//};