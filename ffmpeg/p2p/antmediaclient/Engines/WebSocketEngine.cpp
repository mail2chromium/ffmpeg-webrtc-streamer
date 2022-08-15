//
// Created by zaid.ali on 12/1/2021.
//

#include "WebSocketEngine.h"
#include "p2p/antmediaclient/AntMedia/Constants.h"

WebSocketEngine *WebSocketEngine::webSocketEngine = 0;

void WebSocketEngine::init(std::string url, std::string id) {
    RTC_LOG(LS_ERROR) << "url is " << url;
    WebSocketHelper helper;
    rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_ERROR);
    webrtc_thread = std::thread(&WebSocketHelper::signalthreadentry, helper, url, id);

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        AntMediaWSTrasport.set_access_channels(websocketpp::log::alevel::all);
        AntMediaWSTrasport.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        AntMediaWSTrasport.init_asio();

        // Register our message handler
        AntMediaWSTrasport.set_open_handler(bind(&WebSocketHelper::on_open, helper, &AntMediaWSTrasport, &url, &id, ::_1));
        AntMediaWSTrasport.set_message_handler(bind(&WebSocketHelper::onmessage, helper, &AntMediaWSTrasport, &url, &id, ::_1,::_2));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = AntMediaWSTrasport.get_connection(url, ec);

        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        AntMediaWSTrasport.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        AntMediaWSTrasport.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }

//    AntMediaWSTrasport.stop_listening();
    AntMediaWSTrasport.stop();
    rtc::CleanupSSL();
}

/*TODO; start by creating a string with received char* streamID
 * use constructor method for that
 * then assign the constructed string to local_stream_id variable
 * and in the end, call init method of WebEngine using singleton
 * method*/
void WS_OPEN(char* web_url, char *stream_id) {
    string url_(web_url), id_(stream_id);

    local_web_url = url_;
    local_stream_id = id_;

    WebSocketEngine::getInstance()->init(local_web_url, local_stream_id);
}