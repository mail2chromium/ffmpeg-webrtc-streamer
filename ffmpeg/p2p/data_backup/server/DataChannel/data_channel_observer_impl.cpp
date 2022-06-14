//
// Created by zain on 11/5/2021.
//
#include <iostream>
#include "data_channel_observer_impl.h"
#include <nlohmann/json.hpp>

extern "C" {
    #include "fftools/ffmpeg.h"
}

using json = nlohmann::json;
int8_t* import_buff;

//void GetDesktopResolution(int &horizontal, int &vertical) {
//    RECT desktop;
//    // Get a handle to the desktop window
//    const HWND hDesktop = GetDesktopWindow();
//    // Get the size of screen to the variable desktop
//    GetWindowRect(hDesktop, &desktop);
//    // The top left corner will have coordinates (0,0)
//    // and the bottom right corner will have coordinates
//    // (horizontal, vertical)
//    horizontal = desktop.right;
//    vertical = desktop.bottom;
//}
//
//void setMousePosition(int x, int y, int width, int height) {
//    //client resolution
//    int min = width;
//    int max = height;
//
//    //server
//    int a = 1920; //width
//    int b = 1080; //height
//    GetDesktopResolution(a, b);
//
//    int fx = (((b - a) * (x - min)) / (max - min)) + a;
//    int fy = (((b - a) * (y - min)) / (max - min)) + a;
//
//    SetCursorPos(fx, fy);
//}
//
//void keyPressWithCaps(const char *data, int size) {
//
//    char ch;
//
//    if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
//        //caps is already on nothing todo
//    } else {
//        //ON caps Lock
//        keybd_event(VK_CAPITAL, 0, KEYEVENTF_EXTENDEDKEY, 0); //press caps
//        keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0); //release caps
//
//    }
//
//    if (size == 9) {
//        //only one key
//
//        ch = data[2];
//
//        //press key with caps
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_EXTENDEDKEY, 0);
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_KEYUP, 0);
//
//    } else if (size == 13 && data[2] == 'E'
//               && data[3] == 'n'
//               && data[4] == 't') {
//
//        //kEnter
//        ch = '\n';
//        //press key
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_EXTENDEDKEY, 0);
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_KEYUP, 0);
//    }
//}
//
//void keyPressWithoutCaps(const char *data, int size) {
//
//    if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
//        //caps is already on turn it off
//        keybd_event(VK_CAPITAL, 0, KEYEVENTF_EXTENDEDKEY, 0); //press caps
//        keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0); //release caps
//    }
//
//    char ch;
//    if (size == 9) {
//        //only one key
//
//        ch = data[2];
//
//        //press key
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_EXTENDEDKEY, 0);
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_KEYUP, 0);
//
//    } else if (size == 13 && data[2] == 'E'
//               && data[3] == 'n'
//               && data[4] == 't') {
//        //kEnter
//        ch = '\n';
//        //press key
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_EXTENDEDKEY, 0);
//        keybd_event(VkKeyScanW(ch), 0, KEYEVENTF_KEYUP, 0);
//    }
//}
//
//void tokenize(string s, string del, string array[]) {
//    int index = 0;
//
//    int start = 0;
//    int end = s.find(del);  //first comma
//
//    while (end != -1) {
//        array[index] = s.substr(start, end - start);
//        start = end + del.size();
//        end = s.find(del, start);
//        index++;
//    }
//    array[index] = s.substr(start, end - start);
//
//}
//call back when message is received on data channel
void datachannelobserverimpl::OnMessage(const webrtc::DataBuffer &buffer) {
    RTC_LOG(LS_NONE)
    << "Pointer: " << string((char *) buffer.data.data(), buffer.data.size()) << " received on Data Channel";
    char *data = (char *) buffer.data.data();
    int size = buffer.data.size();
    data_channel->Send(webrtc::DataBuffer("Hello, this is Server "));
}

void delete_dc_import() {
    free(import_buff);
}

void init(int size) {
    import_buff = new int8_t[size]();
}

void fill(int indx, int8_t data) {
    import_buff[indx] = data;
}

void send_to_dc_wrapper(int size)
{
    char hex_[20];
    std::string sobj;

    /*process non-zero stream only when data-channel is
     * available*/
    if(size && data_channel) {
        /*constructing string buffer through concatenation
         * process*/
        for (int j = 0; j < size; j++) {
            sprintf(hex_, "%02x ", import_buff[j]);
            sobj += hex_;
        }
        RTC_LOG(LS_NONE) << sobj.size() << " bytes: " << sobj;

        /*finding end of RTSP stream tag*/
        std::string::size_type loc = sobj.find("ffd9");
        std::cout << loc << std::endl;

        /*sending RTSP stream over data-channel*/
        data_channel->Send(webrtc::DataBuffer(sobj));
    }
}

/*
 * hello
 * Ola
 * HI
 *
 */

