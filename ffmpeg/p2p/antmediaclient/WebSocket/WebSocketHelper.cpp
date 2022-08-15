//
// Created by zain on 11/5/2021.
//

#include "WebSocketHelper.h"

bool AudioCodeIsRequested = false;
bool RequestedAudioTrack = false;

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
std::unique_ptr<rtc::Thread> network_thread;
std::unique_ptr<rtc::Thread> worker_thread;
std::unique_ptr<rtc::PacketSocketFactory> socket_factory;
peerconnectionobserverimpl peer_connection_observer;
rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
//rtc::scoped_refptr<WindowVideoTrackSource> video_source;
//std::unique_ptr<VideoObserver> local_video_observer_;

rtc::scoped_refptr<MyAudioCapturer> AudioCapturer = nullptr;


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

    /*TODO; Generate takeConfiguration command and send answer
    {
            command : "takeConfiguration",
            streamId : "stream1",
            type : "answer",
            sdp : "${SDP_PARAMETER}"
    }*/

    peer_connection->SetLocalDescription(SetSessionDescriptionObserver::Create(), desc);

    if(desc)
        RTC_LOG(LS_NONE) << "Send command to takeConfiguration";

    json message_object;
    message_object["command"] = "takeConfiguration";
    message_object["streamId"] = LOCAL_STREAM_ID;
    message_object["type"] = "answer";

    std::string sdp_str;
    desc->ToString(&sdp_str);
    message_object["sdp"] = sdp_str;
    RTC_LOG(LS_NONE) << "sdp in offer are ";
    RTC_LOG(LS_NONE) << sdp_str;
    RTC_LOG(LS_NONE) << "sending Ant Media " << message_object.dump();

    AntMediaWSTrasport.send(websocket_connection_handler, message_object.dump(), websocketpp::frame::opcode::text);

//    std::string offer_string;
//    desc->ToString(&offer_string);
//
//    RTC_LOG(LS_NONE) << "Local SDP: " << offer_string;
//
//    json message_object;
//    message_object["messageType"] = "answer";
//    json message_payload;
//    message_payload["type"] = "answer";
//    message_payload["sdp"] = offer_string;
//    message_object["payload"] = message_payload;
//    RTC_LOG(LS_NONE) << "Answer Created Successfully. \n";
//    AntMediaWSTrasport.send(websocket_connection_handler, message_object.dump(), websocketpp::frame::opcode::value::text);

}

std::vector<uint8_t> import_buff_4_audio;

void WebSocketHelper::signalthreadentry(std::string url, std::string id) {
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

    peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(),
                                                                  worker_thread.get(),
                                                                  signaling_thread,
                                                                  nullptr,
                                                                  webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                  webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                  webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                  webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                  nullptr /* audio_mixer */,
                                                                  nullptr /* audio_processing */);

    if (RequestedAudioTrack && RequestedVideoTrack) {
        if(raw_data) {
            peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(),
                                                                          worker_thread.get(),
                                                                          signaling_thread,
                                                                          AudioCapturer,
//                                                                          nullptr,
                                                                          webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                          webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                          webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                          webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                          nullptr /* audio_mixer */,
                                                                          nullptr /* audio_processing */);
        }
        else {
            RTC_LOG(LS_ERROR)<<"h264 factory";
            //    Create VideoEncoder H264
            auto video_encoder_factory = std::make_unique<FVideoEncoderFactory>(*this);
            //    VideoEncoderFactory = video_encoder_factory.get();
            //    webrtc::SdpVideoFormat video_format("H264");
            //    video_encoder_factory->CreateVideoEncoder(video_format);
            peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(),
                                                                          worker_thread.get(),
                                                                          signaling_thread,
                                                                          AudioCapturer,
                                                                          webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                          webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                          std::move(video_encoder_factory),
                                                                          webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                          nullptr /* audio_mixer */,
                                                                          nullptr /* audio_processing */);
        }
    }

    if(RequestedAudioTrack && !RequestedVideoTrack) {
        peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(),
                                                                      worker_thread.get(),
                                                                      signaling_thread,
                                                                      AudioCapturer,
                                                                      webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                      webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                      webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                      webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                      nullptr /* audio_mixer */,
                                                                      nullptr /* audio_processing */);
    }

    if(!RequestedAudioTrack && RequestedVideoTrack) {
        if(raw_data) {
            peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(),
                                                                          worker_thread.get(),
                                                                          signaling_thread,
                                                                          nullptr,
                                                                          webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                          webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                          webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                          webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                          nullptr /* audio_mixer */,
                                                                          nullptr /* audio_processing */);
        }
        else {
            RTC_LOG(LS_ERROR)<<"h264 factory";
            //    Create VideoEncoder H264
            auto video_encoder_factory = std::make_unique<FVideoEncoderFactory>(*this);
            //    VideoEncoderFactory = video_encoder_factory.get();
            //    webrtc::SdpVideoFormat video_format("H264");
            //    video_encoder_factory->CreateVideoEncoder(video_format);
            peer_connection_factory = webrtc::CreatePeerConnectionFactory(network_thread.get(),
                                                                          worker_thread.get(),
                                                                          signaling_thread,
                                                                          nullptr,
                                                                          webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                          webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                          std::move(video_encoder_factory),
                                                                          webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                          nullptr /* audio_mixer */,
                                                                          nullptr /* audio_processing */);
        }
    }

    socket_factory.reset();
    signaling_thread->Run();
}

void WebSocketHelper::on_open(client* c, string *url, string *id,
                    websocketpp::connection_hdl hdl) {
    std::cout << "on_message called with hdl: "
              << hdl.lock().get()
              << " and message: " << *url << " " << *id << std::endl;
    websocketpp::lib::error_code ec;

    /*TODO; Sending subscrib command to AntMedia server
    {
        command : "play",
        streamId : "stream1",
        token : "tokenId",
        trackList: [enabledtracksarray],
        subscriberId: "subscriberId",
        subscriberCode: "subscriberCode"

    }*/

    json message_object;
    message_object["command"] = "play";
    message_object["streamId"] = *id;
    cout << message_object.dump() << endl;
    AntMediaWSTrasport.send(hdl, message_object.dump(), websocketpp::frame::opcode::text);

    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }
}

void WebSocketHelper::onmessage(client *s, string *url, string *id, websocketpp::connection_hdl hdl, message_ptr msg) {

    websocket_connection_handler = hdl;
    RTC_LOG(LS_NONE) << "payload: " << msg->get_payload();
    RTC_LOG(LS_NONE) << "url= " << *url << " id=" << *id;

    /*TODO; Check for the response type*/

    /*TODO; in case when server accpets subscriber's
     * stream
     * {
        command : "takeConfiguration",
            streamId : "stream1",
                type : "offer",
                sdp : "${SDP_PARAMETER}"
    }*/

    /*TODO; for iceCandidates against answer
     * {
        command : "takeCandidate",
        streamId : "stream1",
        label : "${CANDIDATE.SDP_MLINE_INDEX}",
        id : "${CANDIDATE.SDP_MID}",
        candidate : "${CANDIDATE.CANDIDATE}"
    }*/

    json message_object = json::parse(msg->get_payload());
    RTC_LOG(LS_NONE) << "Received Ant Media Response: " << message_object;
    std::string command = message_object["command"];

    if(command == "takeCandidate") {
        //TODO; got the iceCandidate
        RTC_LOG(LS_NONE) << "Got the iceCandidate";
        RTC_LOG(LS_NONE) << "Candidates\n";

        std::string candidate = message_object["candidate"];
        std::string sdp_mid = message_object["id"];
        int sdp_mline_index = message_object["label"];

        RTC_LOG(LS_NONE) << candidate << ", id " << sdp_mid << ", label " << sdp_mline_index;

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
    }
    else if(command == "takeConfiguration") {
        RTC_LOG(LS_NONE) << "server accepted subscriber's stream";
        std::string type = message_object["type"];
        if(type == "offer") {
            RTC_LOG(LS_NONE) << "got an offer";
            RTC_LOG(LS_NONE) << "StreamID " << *id << " accepted";

        /*TODO; initialize peer connection, set remote description through offer sdp
         * and send answer sdp configurations to server*/

        webrtc::PeerConnectionInterface::RTCConfiguration configuration;
        // note: two stun servers, with two different ip addresses binded
        // to different tcp ports are needed in the general case
        // https://stackoverflow.com/questions/7594390/why-a-stun-server-needs-two-different-public-ip-addresses
        configuration.type =  webrtc::PeerConnectionInterface::IceTransportsType::kAll;

        webrtc::PeerConnectionInterface::IceServer ice_server;

        ice_server.uri = "stun:stun1.l.google.com:19302";
        configuration.servers.push_back(ice_server);

//        ice_server.uri = "stun:stun2.l.google.com:19305";
//        configuration.servers.push_back(ice_server);

//        {"statusCode": 200, "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
//         "password": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k=", "ttl": "86400",
//         "account_sid": "AC277db84dbd7c874ccd9f78ec1c449f33", "ice_servers":
//           [{"url": "stun:global.stun.twilio.com:3478?transport=udp",
//           "urls": "stun:global.stun.twilio.com:3478?transport=udp"},
//          {"url": "turn:global.turn.twilio.com:3478?transport=udp",
//           "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
//           "urls": "turn:global.turn.twilio.com:3478?transport=udp",
//           "credential": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k="},
//           {"url": "turn:global.turn.twilio.com:3478?transport=tcp",
//            "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
//            "urls": "turn:global.turn.twilio.com:3478?transport=tcp",
//            "credential": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k="},
//            {"url": "turn:global.turn.twilio.com:443?transport=tcp",
//             "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
//             "urls": "turn:global.turn.twilio.com:443?transport=tcp",
//             "credential": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k="}]}

//        ice_server.uri = "turn:global.turn.twilio.com:3478?transport=udp";
//        ice_server.username = "4dad7224de664f0dc89552d6ac7d5b88455d96006c28574cbeee370f842449cf";
//        ice_server.password = "udXjTVjvYi2yjPnGx8bbVX5n7aYif/Igh9OM8M8Jmpg=";
//        configuration.servers.push_back(ice_server);
//
//        ice_server.uri = "turn:global.turn.twilio.com:3478?transport=tcp";
//        ice_server.username = "4dad7224de664f0dc89552d6ac7d5b88455d96006c28574cbeee370f842449cf";
//        ice_server.password = "udXjTVjvYi2yjPnGx8bbVX5n7aYif/Igh9OM8M8Jmpg=";
//        configuration.servers.push_back(ice_server);
//
//        ice_server.uri = "turn:global.turn.twilio.com:443?transport=tcp";
//        ice_server.username = "4dad7224de664f0dc89552d6ac7d5b88455d96006c28574cbeee370f842449cf";
//        ice_server.password = "udXjTVjvYi2yjPnGx8bbVX5n7aYif/Igh9OM8M8Jmpg=";
//        configuration.servers.push_back(ice_server);

//        rtc::scoped_refptr<webrtc::MediaStream> local_stream = webrtc::MediaStream::Create(local_stream_id);
//
//        if(RequestedAudioTrack) {
//            const char audio_label[] = "audio";
//            auto audio_track_local = peer_connection_factory->CreateAudioTrack(audio_label,
//                                                                               peer_connection_factory->CreateAudioSource(
//                                                                                       cricket::AudioOptions()));
//
////            auto AudioTrack =
////                    peer_connection_factory->CreateAudioTrack(audio_label, peer_connection_factory->CreateAudioSource(
////                            cricket::AudioOptions()));
//
//            local_stream->AddTrack(audio_track_local);
//        }

        peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr,
                                                                        &peer_connection_observer);

//        if(RequestedVideoTrack) {
//            const char video_label[] = "video";
//
//            rtc::scoped_refptr<CapturerTrackSource> video_device = CapturerTrackSource::Create();
//
//            if (video_device) {
//                RTC_LOG(LS_NONE) << "Video device Address: " << video_device;
//
//                rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
//                        peer_connection_factory->CreateVideoTrack(video_label,
//                                                                  video_device));
//                local_stream->AddTrack(video_track);
//            }

//            RTC_LOG(LS_NONE) << "Video track size: " << local_stream->GetVideoTracks().size() << "\n";
//        }

//        peer_connection.get()->AddStream(local_stream);

        RTC_LOG(LS_ERROR) << type << " " << message_object["sdp"];
        webrtc::SdpParseError error;
        webrtc::SessionDescriptionInterface *session_description = webrtc::CreateSessionDescription(type, message_object["sdp"], &error);

        if (!session_description) {
            RTC_LOG(LS_ERROR) << "Can't parse received session description message. "
                                   "SdpParseError was: "
                                << error.description;
            return;
        }
        RTC_LOG(LS_ERROR) << " Received session description :" << message_object["sdp"];

        peer_connection->SetRemoteDescription(SetSessionDescriptionObserver::Create(), session_description);

        peer_connection->CreateAnswer(CreateSessionDescriptionObserver::Create(),
                                      webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(1, 1, true, false, true));

        //        peer_connection->CreateOffer(CreateSessionDescriptionObserver::Create(),
//                                     webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(1, 1, true, false, true));
        }
    }

//    if(type == "error") {
//        //TODO; error checking
//        RTC_LOG(LS_NONE) << "got an error which says " << message_object["definition"];
//    }
//    else if(type == "notification") {
//        //TODO; check for definition
//        RTC_LOG(LS_NONE) << "Got a notification message";
//        std::string definition = message_object["definition"];
//        if (definition == "publish_started")
//            RTC_LOG(LS_NONE) << "StreamId " << *id << " has started publishing";
//        else
//            RTC_LOG(LS_NONE) << "notification has definition set to " << definition;
//    }
//    else if(type == "takeCandidate") {
//        //TODO; got the iceCandidate
//        RTC_LOG(LS_NONE) << "Got the iceCandidate";
//        RTC_LOG(LS_NONE) << "Candidates\n";
//        std::string candidate = message_object["candidate"];
//        std::string sdp_mid = message_object["id"];
//        int sdp_mline_index = message_object["label"];
//        RTC_LOG(LS_NONE) << candidate << ", id " << sdp_mid << ", label " << sdp_mline_index;
//
//        webrtc::SdpParseError error;
//        auto candidate_object = webrtc::CreateIceCandidate(sdp_mid, sdp_mline_index, candidate, &error);
//        if (error.description.length() != 0)
//            RTC_LOG(LS_NONE) << "error createicecandidate" << error.description;
//        if (!peer_connection->AddIceCandidate(candidate_object))
//            RTC_LOG(LS_NONE) << "error addicecandidate" << error.description;
//        else {
//            RTC_LOG(LS_NONE) << "Ice candiadte added\n";
//            if (peer_connection->ice_connection_state() ==
//                webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionConnected) {
//                RTC_LOG(LS_NONE) << "Ice Conncted\n";
//            } else if (peer_connection->ice_connection_state() ==
//                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionChecking) {
//                RTC_LOG(LS_NONE) << "Ice Cheking\n";
//            } else if (peer_connection->ice_connection_state() ==
//                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionClosed) {
//                RTC_LOG(LS_NONE) << "Ice Closed\n";
//            } else if (peer_connection->ice_connection_state() ==
//                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionCompleted) {
//                RTC_LOG(LS_NONE) << "Ice Completed\n";
//            } else if (peer_connection->ice_connection_state() ==
//                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected) {
//                RTC_LOG(LS_NONE) << "Ice DIsConncted\n";
//            } else if (peer_connection->ice_connection_state() ==
//                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionFailed) {
//                RTC_LOG(LS_NONE) << "Ice failed\n";
//            } else if (peer_connection->ice_connection_state() ==
//                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionMax) {
//                RTC_LOG(LS_NONE) << "Ice max\n";
//            } else if (peer_connection->ice_connection_state() ==
//                       webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew) {
//                RTC_LOG(LS_NONE) << "Ice new\n";
//            }
//
//            if (peer_connection->peer_connection_state() ==
//                webrtc::PeerConnectionInterface::PeerConnectionState::kConnected) {
//                RTC_LOG(LS_NONE) << "Connected \n";
//            } else if (peer_connection->peer_connection_state() ==
//                       webrtc::PeerConnectionInterface::PeerConnectionState::kClosed) {
//                RTC_LOG(LS_NONE) << "Closd \n";
//            } else if (peer_connection->peer_connection_state() ==
//                       webrtc::PeerConnectionInterface::PeerConnectionState::kConnecting) {
//                RTC_LOG(LS_NONE) << "Connecting \n";
//            } else if (peer_connection->peer_connection_state() ==
//                       webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected) {
//                RTC_LOG(LS_NONE) << "Dis Connected \n";
//            } else if (peer_connection->peer_connection_state() ==
//                       webrtc::PeerConnectionInterface::PeerConnectionState::kFailed) {
//                RTC_LOG(LS_NONE) << "Failed \n";
//            } else if (peer_connection->peer_connection_state() ==
//                       webrtc::PeerConnectionInterface::PeerConnectionState::kNew) {
//                RTC_LOG(LS_NONE) << "New \n";
//            } else {
//                RTC_LOG(LS_NONE) << "not connected\n";
//            }
//        }
//    }
//    else if (type == "takeConfiguration") {
//        type = message_object["type"];
//        if (type == "answer") {
//            //TODO; got the answer now set remote sdp configurations
//            RTC_LOG(LS_NONE) << "Answer received against accepted StreamID " << *id;
//            std::string sdp = message_object["sdp"];
//
//            webrtc::SdpParseError error;
//            webrtc::SessionDescriptionInterface *session_description = webrtc::CreateSessionDescription(type, sdp, &error);
//
//            RTC_LOG(LS_NONE) << session_description->type();
//            if (!session_description) {
//                RTC_LOG(LS_WARNING) << "Can't parse received session description message. "
//                                       "SdpParseError was: "
//                                    << error.description;
//                return;
//            }
//            RTC_LOG(LS_NONE) << " Received session description ";
//            RTC_LOG(LS_NONE) << sdp;
//
//            peer_connection->SetRemoteDescription(SetSessionDescriptionObserver::Create(), session_description);
//        }
//    }
//    else if (type == "start") {
//        RTC_LOG(LS_NONE) << "StreamID " << *id << " accepted";
//        /*TODO; initialize peer connection, create offer sdp and send sdp
//         * configurations to server*/
//
//        webrtc::PeerConnectionInterface::RTCConfiguration configuration;
//        // note: two stun servers, with two different ip addresses binded
//        // to different tcp ports are needed in the general case
//        // https://stackoverflow.com/questions/7594390/why-a-stun-server-needs-two-different-public-ip-addresses
//        configuration.type =  webrtc::PeerConnectionInterface::IceTransportsType::kAll;
//
//        webrtc::PeerConnectionInterface::IceServer ice_server;
//
//        ice_server.uri = "stun:stun1.l.google.com:19302";
//        configuration.servers.push_back(ice_server);
//
////        ice_server.uri = "stun:stun2.l.google.com:19305";
////        configuration.servers.push_back(ice_server);
//
////        {"statusCode": 200, "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
////         "password": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k=", "ttl": "86400",
////         "account_sid": "AC277db84dbd7c874ccd9f78ec1c449f33", "ice_servers":
////           [{"url": "stun:global.stun.twilio.com:3478?transport=udp",
////           "urls": "stun:global.stun.twilio.com:3478?transport=udp"},
////          {"url": "turn:global.turn.twilio.com:3478?transport=udp",
////           "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
////           "urls": "turn:global.turn.twilio.com:3478?transport=udp",
////           "credential": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k="},
////           {"url": "turn:global.turn.twilio.com:3478?transport=tcp",
////            "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
////            "urls": "turn:global.turn.twilio.com:3478?transport=tcp",
////            "credential": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k="},
////            {"url": "turn:global.turn.twilio.com:443?transport=tcp",
////             "username": "e0c73723879a6cd5adb709c35dcd07ff916281a5c05847aa5d4785e60cc74eca",
////             "urls": "turn:global.turn.twilio.com:443?transport=tcp",
////             "credential": "r/QPYaz8/PY2FB0hqiXoovZ4DXsyLYTY41qhRnwsj7k="}]}
//
////        ice_server.uri = "turn:global.turn.twilio.com:3478?transport=udp";
////        ice_server.username = "4dad7224de664f0dc89552d6ac7d5b88455d96006c28574cbeee370f842449cf";
////        ice_server.password = "udXjTVjvYi2yjPnGx8bbVX5n7aYif/Igh9OM8M8Jmpg=";
////        configuration.servers.push_back(ice_server);
////
////        ice_server.uri = "turn:global.turn.twilio.com:3478?transport=tcp";
////        ice_server.username = "4dad7224de664f0dc89552d6ac7d5b88455d96006c28574cbeee370f842449cf";
////        ice_server.password = "udXjTVjvYi2yjPnGx8bbVX5n7aYif/Igh9OM8M8Jmpg=";
////        configuration.servers.push_back(ice_server);
////
////        ice_server.uri = "turn:global.turn.twilio.com:443?transport=tcp";
////        ice_server.username = "4dad7224de664f0dc89552d6ac7d5b88455d96006c28574cbeee370f842449cf";
////        ice_server.password = "udXjTVjvYi2yjPnGx8bbVX5n7aYif/Igh9OM8M8Jmpg=";
////        configuration.servers.push_back(ice_server);
//
//        rtc::scoped_refptr<webrtc::MediaStream> local_stream = webrtc::MediaStream::Create(local_stream_id);
//
//        if(RequestedAudioTrack) {
//            const char audio_label[] = "audio";
//            auto audio_track_local = peer_connection_factory->CreateAudioTrack(audio_label,
//                                                                               peer_connection_factory->CreateAudioSource(
//                                                                                       cricket::AudioOptions()));
//
////            auto AudioTrack =
////                    peer_connection_factory->CreateAudioTrack(audio_label, peer_connection_factory->CreateAudioSource(
////                            cricket::AudioOptions()));
//
//            local_stream->AddTrack(audio_track_local);
//        }
//
//        peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr,
//                                                                        &peer_connection_observer);
//
//        if(RequestedVideoTrack) {
//            const char video_label[] = "video";
//
//            rtc::scoped_refptr<CapturerTrackSource> video_device = CapturerTrackSource::Create();
//
//            if (video_device) {
//                RTC_LOG(LS_NONE) << "Video device Address: " << video_device;
//
//                rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
//                        peer_connection_factory->CreateVideoTrack(video_label,
//                                                                  video_device));
//                local_stream->AddTrack(video_track);
//            }
//
//            RTC_LOG(LS_NONE) << "Video track size: " << local_stream->GetVideoTracks().size() << "\n";
//        }
//
//        peer_connection.get()->AddStream(local_stream);
//
////        webrtc::SdpParseError error;
////        webrtc::SessionDescriptionInterface *session_description = webrtc::CreateSessionDescription(type, sdp, &error);
////
////        if (!session_description) {
////            RTC_LOG(LS_WARNING) << "Can't parse received session description message. "
////                                   "SdpParseError was: "
////                                << error.description;
////            return;
////        }
////        RTC_LOG(LS_NONE) << " Received session description :" << sdp;
//
//////        peer_connection->SetRemoteDescription(SetSessionDescriptionObserver::Create(), session_description);
////        peer_connection->CreateAnswer(CreateSessionDescriptionObserver::Create(),
////                                      webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(0, 0, true, false, true));
//        peer_connection->CreateOffer(CreateSessionDescriptionObserver::Create(),
//                                     webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(0, 0, true, false, true));
//    }

}

void WebSocketHelper::ForceKeyFrame() {
    std::cout << "Force Key frame." << std::endl;
}

void WebSocketHelper::SetRate(uint32_t BitrateKbps, uint32_t Framerate) {

    RTC_LOG(LS_ERROR) << "Bitrate : " << BitrateKbps << " , FrameRate: " << Framerate;

}

void AudioTrackIsRequired() {
    RequestedAudioTrack = true;
}

void SendAudioPackets(int pkt_size) {
    if (AudioCapturer) {
        AudioCapturer->ProcessPacket(import_buff_4_audio.data(), pkt_size, AudioCodeIsRequested);
    } else {
        RTC_LOG(LS_ERROR) << "Audio capturer not init yet";
    }
}

void InitializeAudioPackets(int siz) {
    webrtc::voe::ffmpeg_encoded_buffer_size = siz;
}

void FillAudioPackets(int pos, uint8_t data) {
    import_buff_4_audio.push_back(data);
}

void DeleteAudioPackets() {
    import_buff_4_audio.clear();
}

void SetPreEncoding4Audio() {
    webrtc::voe::is_pre_encoding = true;
    AudioCodeIsRequested = true;
}

void SetPTS4Audio(int64_t pts) {
    webrtc::voe::ffmpeg_av_pkt_pts = pts;
}