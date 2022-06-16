//
// Created by ottoman on 12/06/2021.
//

#ifndef SENDVIDEOTOWEB_PEER_CONNECTION_OBSERVER_IMPL_H
#define SENDVIDEOTOWEB_PEER_CONNECTION_OBSERVER_IMPL_H

#include <websocketpp/server.hpp>
#include "../src/observers.h"
#include "../src/precmp.h"
#include "../DataChannel/data_channel_observer_impl.h"
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> websocketserver;
typedef websocketserver::message_ptr message_ptr;

extern websocketpp::connection_hdl websocket_connection_handler;
extern websocketserver ws_server;
extern rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
extern rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track;


class peerconnectionobserverimpl : public PeerConnectionObserver {
public:
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;

    void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override;

    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

    void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
};

#endif //SENDVIDEOTOWEB_PEER_CONNECTION_OBSERVER_IMPL_H
