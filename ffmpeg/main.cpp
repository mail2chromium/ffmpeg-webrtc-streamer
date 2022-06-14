//
// Created by webrtc-2 on 5/31/22.
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

int main(void) {

    ////////////////////// MP4

    char *writeMp4[28] = {"ffmpeg",
                          "-y",
                          "-re",
                          "-f",
                          "pulse",
                          "-i",
                          "default",
                          "-f",
                          "v4l2",
                          "-i",
                          "/dev/video0",
                          "-acodec",
                          "aac",
                          "-strict",
                          "-2",
                          "-ac",
                          "1",
                          "-b:a",
                          "64k",
                          "-vcodec",
                          "libx264",
                          "-b:v",
                          "300k",
                          "-r",
                          "30",
                          "-g",
                          "30",
                          "prueba1.mp4"};

//    thread th3(&ffmpeg_main, 28, writeMp4);


    /////////////////////////

    /*Command buffer to record camera feed into mp4 file*/
//    char* InputAVI2RawYUV[10] = {"ffmpeg", "-re", "-y", "-i", "input.avi", "-c:v", "rawvideo", "-pixel_format", "yuv420p", "output.yuv"};


    char *Cam2Mp4RecordingRawYUV[38] = {"ffmpeg", "-re", "-y", "-f", "v4l2", "-framerate", "25", "-video_size",
                                        "640x480", "-i", "/dev/video0", "-pix_fmt", "yuv420p",
                                        "-preset", "ultrafast", "-tune", "zerolatency", "-r", "10", "-b:v", "512k",
                                        "-s", "640x480", "-c:v", "rawvideo", "-ac", "2",
                                        "-ab", "32k", "-ar", "44100", "-vf", "format=yuv420p", "-update", "1",
                                        "-flush_packets", "0", "output.yuv"};

    thread th1(&ffmpeg_main, 38, Cam2Mp4RecordingRawYUV);

//////////////// VIDEO
    char *Cam2Mp4RecordingEncoded[36] = {"ffmpeg", "-re", "-y", "-f", "v4l2", "-framerate", "25", "-video_size",
                                         "640x480", "-i", "/dev/video0", "-pix_fmt", "yuv420p",
                                         "-preset", "ultrafast", "-tune", "zerolatency", "-r", "10", "-b:v", "512k",
                                         "-s", "640x480", "-ac", "2", "-ab", "32k", "-ar", "44100", "-vf",
                                         "format=yuv420p",
                                         "-update", "1", "-flush_packets", "0", "output.yuv"};
//    thread th1(&ffmpeg_main, 36, Cam2Mp4RecordingEncoded);

    ////// AUDIO
//    char *Mic2Wav[15] = {"ffmpeg", "-re", "-f", "pulse", "-i", "default", "-ac", "1", "-packetsize", "480",
//                         "-sample_rate", "48000", "-af", "pan=mono|c0=FL", "output.wav"};
//    thread th2(&ffmpeg_main, 15, Mic2Wav);
////

//    AudioCapturer = new rtc::RefCountedObject<MyAudioCapturer>();
//    AudioCapturer->Init();
//    AudioCapturer->InitRecording();

//    thread th(&readAudio, AudioCapturer);

    WebSocketEngine webSocketEngine;
    webSocketEngine.init();

//    th3.join();
    th1.join();
    return 0;
}