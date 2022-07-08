//
// Created by Ottoman on 5/31/22.
//

#include "api/media_types.h"
#include "rtc_base/arraysize.h"
#include "p2p/server/Engines/WebSocketEngine.h"
#include "p2p/server/Audiocapturer/MyAudioCapturer.h"

extern "C" {
#include "fftools/ffmpeg.h"
}

#include <iostream>
#include <cstdlib>
#include <thread>

#include "p2p/server/Audiocapturer/wavFileReader.h"

using namespace std;

static void readAudio(rtc::scoped_refptr<MyAudioCapturer> AudioCapturer) {
    wavFileReader fileReader;
    while (true) {
        fileReader.readFile(AudioCapturer);
        sleep(5);
    }
}

int main(int argc, char **argv) {

    thread th1(&ffmpeg_main, argc, argv);

    // Main Audio Capturer Bridge
    AudioCapturer = new rtc::RefCountedObject<MyAudioCapturer>();
    AudioCapturer->Init();
    AudioCapturer->InitRecording();

    // Main Signaling Bridge
    WebSocketEngine webSocketEngine;
    webSocketEngine.init();

    th1.join();

    return 0;
}