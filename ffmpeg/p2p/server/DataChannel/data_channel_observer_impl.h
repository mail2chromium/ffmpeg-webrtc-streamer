//
// Created by zain on 11/5/2021.
//

#ifndef SENDVIDEOTOWEB_DATA_CHANNEL_OBSERVER_IMPL_H
#define SENDVIDEOTOWEB_DATA_CHANNEL_OBSERVER_IMPL_H


#include <iostream>
#include <string>
#include "../peer_connection_observer_impl/peer_connection_observer_impl.h"
using namespace std;

class datachannelobserverimpl : public DataChannelObserver {
public:
    void OnMessage(const webrtc::DataBuffer &buffer) override;
};

#endif //SENDVIDEOTOWEB_DATA_CHANNEL_OBSERVER_IMPL_H
