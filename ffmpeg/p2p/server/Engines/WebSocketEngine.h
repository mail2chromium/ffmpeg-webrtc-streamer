//
// Created by ottoman on 12/1/2021.
//
#pragma once
#ifndef SENDVIDEOTOWEB_WEBSOCKETENGINE_H
#define SENDVIDEOTOWEB_WEBSOCKETENGINE_H

#include <iostream>
#include "../src/WebRTCProxyCommon.h"
#include "../DataChannel/data_channel_observer_impl.h"
#include "../WebSocket/WebSocketHelper.h"


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;


// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")


class WebSocketEngine {
private:
    std::thread webrtc_thread;

public:
    WebSocketEngine(){}

    void init( );

};


#endif //SENDVIDEOTOWEB_WEBSOCKETENGINE_H
