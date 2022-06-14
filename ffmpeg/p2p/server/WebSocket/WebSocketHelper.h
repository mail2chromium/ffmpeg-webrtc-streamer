//
// Created by zain on 11/5/2021.
//
#pragma once
#ifndef SENDVIDEOTOWEB_WEBSOCKETHELPER_H
#define SENDVIDEOTOWEB_WEBSOCKETHELPER_H

#include "../Audiocapturer/MyAudioCapturer.h"

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

using json = nlohmann::json;
extern rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
extern std::unique_ptr<rtc::Thread> network_thread;
extern std::unique_ptr<rtc::Thread> worker_thread;
extern std::unique_ptr<rtc::PacketSocketFactory> socket_factory;
extern peerconnectionobserverimpl peer_connection_observer;
extern rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;


//extern "C" void send_to_vc_wrapper(int);
//extern "C" void init(int);
//extern "C" void fill(int, uint8_t);
//extern "C" void delete_vc_import();
extern "C" void display_pkt_type(int);

extern rtc::scoped_refptr<MyAudioCapturer> AudioCapturer;

static void signalthreadentry() {


    if (AudioCapturer) {
        RTC_LOG(LS_ERROR) << "init capturer";
    } else {
        RTC_LOG(LS_ERROR) << "not init capturer";
    }

// create the peerconnectionfactory.
    rtc::InitializeSSL();

    network_thread.reset(rtc::Thread::CreateWithSocketServer().release());
    worker_thread.reset(rtc::Thread::Create().release());
    rtc::Thread *signaling_thread = rtc::Thread::Current();
    network_thread->Start();
    worker_thread->Start();


//    auto VideoEncoderFactoryStrong = std::make_unique<FVideoEncoderFactory>();
    // #HACK: Keep a pointer to the Video encoder factory, so we can use it to figure out the
    // FClientSession <-> FakeVideoEncoder relationship later on

//    FVideoEncoderFactory*  VideoEncoderFactory = VideoEncoderFactoryStrong.get();

    peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(), worker_thread.get(),
                                                                  signaling_thread,
//                                                                  AudioCapturer,
                                                                  nullptr,
                                                                  webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                  webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                  webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                  webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                  nullptr /* audio_mixer */,
                                                                  nullptr /* audio_processing */);

    socket_factory.reset();
    signaling_thread->Run();
}


static void onwebsocketmessage_updated(websocketserver *s, websocketpp::connection_hdl hdl, message_ptr msg) {

    websocket_connection_handler = hdl;
    RTC_LOG(LS_NONE) << "payload: " << msg->get_payload();
    json message_object = json::parse(msg->get_payload());
    RTC_LOG(LS_NONE) << "Received Offer message: " << message_object;
    std::string type = message_object["messageType"];

    if (type == "offer") {

        std::string sdp = message_object["payload"]["sdp"];

        webrtc::PeerConnectionInterface::RTCConfiguration configuration;
        // note: two stun servers, with two different ip addresses binded
        // to different tcp ports are needed in the general case
        // https://stackoverflow.com/questions/7594390/why-a-stun-server-needs-two-different-public-ip-addresses
        webrtc::PeerConnectionInterface::IceServer ice_server;
        ice_server.uri = "stun:stun1.l.google.com:19302";
        configuration.servers.push_back(ice_server);
        ice_server.uri = "stun:stun2.l.google.com:19305";
        configuration.servers.push_back(ice_server);


        const char audio_label[] = "audio";
        const char video_label[] = "video";

        auto audio_track_local = peer_connection_factory->CreateAudioTrack(audio_label,
                                                                           peer_connection_factory->CreateAudioSource(
                                                                                   cricket::AudioOptions()));

//        auto AudioTrack =
//                peer_connection_factory->CreateAudioTrack(audio_label, peer_connection_factory->CreateAudioSource(
//                        cricket::AudioOptions()));


        peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr,
                                                                        &peer_connection_observer);


        rtc::scoped_refptr<webrtc::MediaStream> local_stream = webrtc::MediaStream::Create("LocalStreamId");

        rtc::scoped_refptr<CapturerTrackSource> video_device = CapturerTrackSource::Create();

        if (video_device) {
            RTC_LOG(LS_NONE) << "Video device Address: " << video_device;

            rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
                    peer_connection_factory->CreateVideoTrack(video_label,
                                                              video_device));
            local_stream->AddTrack(video_track);
        }

        local_stream->AddTrack(audio_track_local);
        peer_connection.get()->AddStream(local_stream);

        RTC_LOG(LS_NONE) << "Video track size: " << local_stream->GetVideoTracks().size() << "\n";

        webrtc::SdpParseError error;
        webrtc::SessionDescriptionInterface *session_description = webrtc::CreateSessionDescription(type, sdp, &error);

        if (!session_description) {
            RTC_LOG(LS_WARNING) << "Can't parse received session description message. "
                                   "SdpParseError was: "
                                << error.description;
            return;
        }
        RTC_LOG(LS_NONE) << " Received session description :" << sdp;

        peer_connection->SetRemoteDescription(SetSessionDescriptionObserver::Create(), session_description);
        peer_connection->CreateAnswer(CreateSessionDescriptionObserver::Create(),
                                      webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(0, 0, true, false, true));

    } else if (type == "candidate") {
        RTC_LOG(LS_NONE) << "Candidates\n";
        std::string candidate = message_object["payload"]["candidate"];
        std::string sdp_mid = message_object["payload"]["sdpMid"];
        int sdp_mline_index = message_object["payload"]["sdpMLineIndex"];
        RTC_LOG(LS_NONE) << candidate << ", sdpMid " << sdp_mid << ", sdpMLineIndex " << sdp_mline_index;


        webrtc::SdpParseError error;
        auto candidate_object = webrtc::CreateIceCandidate(sdp_mid, sdp_mline_index, candidate, &error);
        if (error.description.length() != 0)
            RTC_LOG(LS_NONE) << "error createicecandidate" << error.description;
        if (!peer_connection->AddIceCandidate(candidate_object))
            RTC_LOG(LS_NONE) << "error addicecandidate" << error.description;
        else {
            RTC_LOG(LS_NONE) << "Ice candiadte added\n";
            if (peer_connection->ice_connection_state() ==
                webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionConnected) {
                RTC_LOG(LS_NONE) << "Ice Conncted\n";
            } else if (peer_connection->ice_connection_state() ==
                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionChecking) {
                RTC_LOG(LS_NONE) << "Ice Cheking\n";
            } else if (peer_connection->ice_connection_state() ==
                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionClosed) {
                RTC_LOG(LS_NONE) << "Ice Closed\n";
            } else if (peer_connection->ice_connection_state() ==
                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionCompleted) {
                RTC_LOG(LS_NONE) << "Ice Completed\n";
            } else if (peer_connection->ice_connection_state() ==
                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected) {
                RTC_LOG(LS_NONE) << "Ice DIsConncted\n";
            } else if (peer_connection->ice_connection_state() ==
                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionFailed) {
                RTC_LOG(LS_NONE) << "Ice failed\n";
            } else if (peer_connection->ice_connection_state() ==
                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionMax) {
                RTC_LOG(LS_NONE) << "Ice max\n";
            } else if (peer_connection->ice_connection_state() ==
                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew) {
                RTC_LOG(LS_NONE) << "Ice new\n";
            }


            if (peer_connection->peer_connection_state() ==
                webrtc::PeerConnectionInterface::PeerConnectionState::kConnected) {
                RTC_LOG(LS_NONE) << "Connected \n";
            } else if (peer_connection->peer_connection_state() ==
                       webrtc::PeerConnectionInterface::PeerConnectionState::kClosed) {
                RTC_LOG(LS_NONE) << "Closd \n";
            } else if (peer_connection->peer_connection_state() ==
                       webrtc::PeerConnectionInterface::PeerConnectionState::kConnecting) {
                RTC_LOG(LS_NONE) << "Connecting \n";
            } else if (peer_connection->peer_connection_state() ==
                       webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected) {
                RTC_LOG(LS_NONE) << "Dis Connected \n";
            } else if (peer_connection->peer_connection_state() ==
                       webrtc::PeerConnectionInterface::PeerConnectionState::kFailed) {
                RTC_LOG(LS_NONE) << "Failed \n";
            } else if (peer_connection->peer_connection_state() ==
                       webrtc::PeerConnectionInterface::PeerConnectionState::kNew) {
                RTC_LOG(LS_NONE) << "New \n";
            } else {
                RTC_LOG(LS_NONE) << "not connected\n";
            }

        }


    } else {
        RTC_LOG(LS_NONE) << "unrecognized websocket message type.";
    }

}

#endif //SENDVIDEOTOWEB_WEBSOCKETHELPER_H
