#pragma once

//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
//#define _CRT_SECURE_NO_WARNINGS
//#include <Windows.h>

#include <api/peer_connection_interface.h>
#include <pc/peer_connection_factory.h>
#include <rtc_base/physical_socket_server.h>
#include <rtc_base/fake_network.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/thread.h>
#include <p2p/client/basic_port_allocator.h>
#include "p2p/base/basic_packet_socket_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include <api/peer_connection_interface.h>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>


#include <functional>
#include <iostream>
#include <thread>