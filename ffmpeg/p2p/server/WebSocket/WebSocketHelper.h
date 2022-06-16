//
// Created by ottoman on 12/06/2021.
//

#pragma once
#ifndef SENDVIDEOTOWEB_WEBSOCKETHELPER_H
#define SENDVIDEOTOWEB_WEBSOCKETHELPER_H

#include <nlohmann/json.hpp>
#include "../src/WebRTCProxyCommon.h"
#include <rtc_base/ssl_adapter.h>
#include <api/video/video_stream_encoder_interface.h>
#include "../peer_connection_observer_impl/peer_connection_observer_impl.h"
#include "common_video/include/i420_buffer_pool.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "media/base/adapted_video_track_source.h"
#include "rtc_base/async_invoker.h"
#include "rtc_base/checks.h"
#include "rtc_base/thread.h"
#include "rtc_base/timestamp_aligner.h"
#include "rtc_base/logging.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "../Capturer/CapturerTrackSource.h"
#include "media/engine/multiplex_codec_factory.h"
#include "Codec/rtcVideoEncoder.h"

using json = nlohmann::json;
extern rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
extern std::unique_ptr<rtc::Thread> network_thread;
extern std::unique_ptr<rtc::Thread> worker_thread;
extern std::unique_ptr<rtc::PacketSocketFactory> socket_factory;
extern peerconnectionobserverimpl peer_connection_observer;
extern rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;

class WebSocketHelper : public IVideoEncoderObserver {

private:
    FVideoEncoderFactory* VideoEncoderFactory = nullptr;

public:
    // IVideoEncoderObserver
    void ForceKeyFrame() override;
    void SetRate(uint32_t BitrateKbps, uint32_t Framerate) override;

    void signalthreadentry();
    void onwebsocketmessage_updated(websocketserver *s, websocketpp::connection_hdl hdl, message_ptr msg);
    static int raw_data;

};

#endif //SENDVIDEOTOWEB_WEBSOCKETHELPER_H
