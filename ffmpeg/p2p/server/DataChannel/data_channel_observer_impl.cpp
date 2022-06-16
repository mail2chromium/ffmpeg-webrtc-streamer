//
// Created by ottoman on 12/06/2021.
//

#include "data_channel_observer_impl.h"

//call back when message is received on data channel
void datachannelobserverimpl::OnMessage(const webrtc::DataBuffer &buffer) {
    RTC_LOG(LS_NONE) << "ondatachannelmessage" ;

    RTC_LOG(LS_NONE) << string((char *) buffer.data.data(), buffer.data.size()) ;
    RTC_LOG(LS_NONE) << "Input reply: ";
    string reply = "";
    cin >> reply;
    webrtc::DataBuffer answer(reply);
    data_channel->Send(answer);
}
