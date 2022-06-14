//
// Created by zain on 11/5/2021.
//

#ifndef SENDVIDEOTOWEB_DATA_CHANNEL_OBSERVER_IMPL_H
#define SENDVIDEOTOWEB_DATA_CHANNEL_OBSERVER_IMPL_H


#include <iostream>
#include <string>
#include "p2p/data_backup/server/peer_connection_observer_impl/peer_connection_observer_impl.h"
using namespace std;

extern "C" void init(int);
extern "C" void send_to_dc_wrapper(int);
extern "C" void delete_dc_import();
extern "C" void fill(int, int8_t);

class datachannelobserverimpl : public DataChannelObserver {
public:
    void OnMessage(const webrtc::DataBuffer &buffer) override;

};

#endif //SENDVIDEOTOWEB_DATA_CHANNEL_OBSERVER_IMPL_H
