//
// Created by ottoman on 12/06/2021.
//

#include "WebSocketHelper.h"

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
    RTC_LOG(LS_NONE) << "Answer Created Successfully. \n";
    ws_server.send(websocket_connection_handler, message_object.dump(), websocketpp::frame::opcode::value::text);

}


std::vector<uint8_t> import_buff_4_audio;

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