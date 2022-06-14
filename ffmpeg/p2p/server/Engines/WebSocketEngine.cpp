//
// Created by ottoman on 12/1/2021.
//

#include "WebSocketEngine.h"


void WebSocketEngine::init() {

    rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_ERROR);
    webrtc_thread = std::thread(signalthreadentry);

    // in a real game server, you would run the websocket server as a separate thread so your main
    // process can handle the game loop.

    try {
        // Set logging settings
        ws_server.set_access_channels(websocketpp::log::alevel::all);
        ws_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        ws_server.init_asio();

        // Register our message handler
        ws_server.set_message_handler(bind(&onwebsocketmessage_updated, &ws_server, ::_1, ::_2));

        // To reuse the address
        ws_server.set_reuse_addr(true);

        // Listen on port 9002
        ws_server.listen(4321);

        // Start the server accept loop
        ws_server.start_accept();

        // Start the ASIO io_service run loop
        ws_server.run();

    } catch (websocketpp::exception const &e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        RTC_LOG(LS_NONE) << "other exception";
    }

    ws_server.stop_listening();
    ws_server.stop();

    rtc::CleanupSSL();
}