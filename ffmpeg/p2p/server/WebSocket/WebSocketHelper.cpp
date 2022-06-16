//
// Created by ottoman on 12/06/2021.
//

#include "WebSocketHelper.h"
//#include "modules/video_coding/codecs/h264/win/h264_mf_factory.h"

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
std::unique_ptr<rtc::Thread> network_thread;
std::unique_ptr<rtc::Thread> worker_thread;
std::unique_ptr<rtc::PacketSocketFactory> socket_factory;
peerconnectionobserverimpl peer_connection_observer;
rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
//rtc::scoped_refptr<WindowVideoTrackSource> video_source;
//std::unique_ptr<VideoObserver> local_video_observer_;


//void RegisterOnLocalI420FrameReady_(
//        I420FRAMEREADY_CALLBACK callback) {
//    if (local_video_observer_) {
//        RTC_LOG(LS_NONE) << "\nRegister on local 420 \n";
//        local_video_observer_->SetVideoCallback(callback);
//    } else {
//        RTC_LOG(LS_NONE) << "\nRegister on local 420 Failed \n";
//    }
//}


static void func(const uint8_t *data_y,
                 const uint8_t *data_u,
                 const uint8_t *data_v,
                 const uint8_t *data_a,
                 int stride_y,
                 int stride_u,
                 int stride_v,
                 int stride_a,
                 uint32_t width,
                 uint32_t height) {

    RTC_LOG(LS_NONE) << "\n\nHeight: " << height;
    RTC_LOG(LS_NONE) << "Width: " << width;
    RTC_LOG(LS_NONE) << "data_y: " << &data_y;
    RTC_LOG(LS_NONE) << "data_u: " << &data_u;
    RTC_LOG(LS_NONE) << "data_v: " << &data_v;
    RTC_LOG(LS_NONE) << "stride_y: " << stride_y;
    RTC_LOG(LS_NONE) << "stride_u: " << stride_u;
    RTC_LOG(LS_NONE) << "stride_v: " << stride_v;

}

void CreateSessionDescriptionObserver::OnSuccess(webrtc::SessionDescriptionInterface *desc) {

    RTC_LOG(LS_NONE) << "OnSuccess called\n";


    peer_connection->SetLocalDescription(
            SetSessionDescriptionObserver::Create(), desc);
    std::string offer_string;
    desc->ToString(&offer_string);

    RTC_LOG(LS_NONE) << "Local SDP: " << offer_string;

    json message_object;
    message_object["messageType"] = "answer";
    json message_payload;
    message_payload["type"] = "answer";
    message_payload["sdp"] = offer_string;
    message_object["payload"] = message_payload;
    RTC_LOG(LS_ERROR) << "Answer Created Successfully. \n" << offer_string;

    ws_server.send(websocket_connection_handler, message_object.dump(), websocketpp::frame::opcode::value::text);

}

void WebSocketHelper::signalthreadentry() {
// create the peerconnectionfactory.
    rtc::InitializeSSL();

    network_thread.reset(rtc::Thread::CreateWithSocketServer().release());
    worker_thread.reset(rtc::Thread::Create().release());
    rtc::Thread *signaling_thread = rtc::Thread::Current();
    network_thread->Start();
    worker_thread->Start();

    if (raw_data == 1) {

        peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(), worker_thread.get(),
                                                                      signaling_thread,
                                                                      nullptr,
                                                                      webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                      webrtc::CreateBuiltinAudioDecoderFactory(),
//                                                                      std::move(video_encoder_factory),
                                                                      webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                      webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                      nullptr /* audio_mixer */,
                                                                      nullptr /* audio_processing */);

    } else if (raw_data == 0) {
        RTC_LOG(LS_ERROR)<<"h264 factory";

        //    Create VideoEncoder H264
        auto video_encoder_factory = std::make_unique<FVideoEncoderFactory>(*this);
//    VideoEncoderFactory = video_encoder_factory.get();

//    webrtc::SdpVideoFormat video_format("H264");
//    video_encoder_factory->CreateVideoEncoder(video_format);


        peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(), worker_thread.get(),
                                                                      signaling_thread,
                                                                      nullptr,
                                                                      webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                      webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                      std::move(video_encoder_factory),
//                                                                  webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                      webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                      nullptr /* audio_mixer */,
                                                                      nullptr /* audio_processing */);

    }


    socket_factory.reset();
    signaling_thread->Run();
}

void WebSocketHelper::onwebsocketmessage_updated(websocketserver *s, websocketpp::connection_hdl hdl, message_ptr msg) {

    websocket_connection_handler = hdl;
    RTC_LOG(LS_NONE) << "payload: " << msg->get_payload();
    json message_object = json::parse(msg->get_payload());
    RTC_LOG(LS_NONE) << "Received Offer message: " << message_object;
    std::string type = message_object["messageType"];

    if (type == "offer") {

        std::string sdp = message_object["payload"]["sdp"];
        RTC_LOG(LS_ERROR) << "SDP offer: " << sdp;

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

void WebSocketHelper::ForceKeyFrame() {
    std::cout << "Force Key frame." << std::endl;
}

void WebSocketHelper::SetRate(uint32_t BitrateKbps, uint32_t Framerate) {

    RTC_LOG(LS_ERROR) << "Bitrate : " << BitrateKbps << " , FrameRate: " << Framerate;

}
