//
// Created by zaid.ali on 12/1/2021.
//
#pragma once
#ifndef SENDVIDEOTOWEB_WEBSOCKETENGINE_H
#define SENDVIDEOTOWEB_WEBSOCKETENGINE_H

#include <iostream>
#include "../src/WebRTCProxyCommon.h"
//#include "../DataChannel/data_channel_observer_impl.h"
#include "../WebSocket/WebSocketHelper.h"
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

extern "C" void WS_OPEN(char* web_url, char *stream_id);

//TODO; use singleton method to init webengine
class WebSocketEngine {
//private:
public:
    std::thread webrtc_thread;

    //Get Instance Method
    static WebSocketEngine *getInstance() {
        if(!webSocketEngine)
            webSocketEngine = new WebSocketEngine;
        return webSocketEngine;
    }

    //Singleton Instance
    static WebSocketEngine *webSocketEngine;

    //Constructor
    WebSocketEngine(){}

    //WebEngine start method
    void init(std::string, std::string);

};

#endif //SENDVIDEOTOWEB_WEBSOCKETENGINE_H
