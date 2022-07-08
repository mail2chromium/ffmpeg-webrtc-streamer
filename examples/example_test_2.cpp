//
// Created by ottoman on 5/31/22.
//
#include "api/media_types.h"
#include "rtc_base/arraysize.h"
#include "p2p/server/Engines/WebSocketEngine.h"
#include "p2p/server/Audiocapturer/MyAudioCapturer.h"
extern FILE *outfile = fopen("raw_output.wav", "a+");
extern "C" {
#include "fftools/ffmpeg.h"
}
#include <iostream>
#include <cstdlib>
#include <thread>
#include "p2p/server/Audiocapturer/wavFileReader.h"
//using namespace std;
wavFileReader *wavFileReader::instance = 0;
static void readAudio(rtc::scoped_refptr<MyAudioCapturer> AudioCapturer) {
//    while (true) {
    wavFileReader::getinstance()->readFile(AudioCapturer);
//        sleep(5);
//    }
}
int main(void) {
    ////////////////////// MP4
//    char *writeMp4[28] = {"ffmpeg",
//                          "-y",
//                          "-re",
//                          "-f",
//                          "pulse",
//                          "-i",
//                          "default",
//                          "-f",
//                          "v4l2",
//                          "-i",
//                          "/dev/video0",
//                          "-acodec",
//                          "aac",
//                          "-strict",
//                          "-2",
//                          "-ac",
//                          "1",
//                          "-b:a",
//                          "64k",
//                          "-vcodec",
//                          "libx264",
//                          "-b:v",
//                          "300k",
//                          "-r",
//                          "30",
//                          "-g",
//                          "30",
//                          "prueba1.mp4"};
//
//    thread th3(&ffmpeg_main, 28, writeMp4);
    /////////////////////////
    /*Command buffer to record camera feed into mp4 file*/
//    char* InputAVI2RawYUV[10] = {"ffmpeg", "-re", "-y", "-i", "input.avi", "-c:v", "rawvideo", "-pixel_format", "yuv420p", "output.yuv"};
//    char *Mic2Wav[18] = {"ffmpeg", "-y", "-re", "-f", "pulse", "-i", "default", "-ac", "2",
//                         "-sample_rate", "48000", "-b:a", "64000", "-c:a", "opus", "-strict", "-2", "output_audio.ogg"};
//    thread th2(&ffmpeg_main, 18, Mic2Wav);
//    char *Mic2WavMod[30] = {"ffmpeg", "-y", "-re", "-use_wallclock_as_timestamps", "1", "-fflags",
//                          "+genpts", "-max_delay", "200", "-thread_queue_size", "1024", "-f",
//                          "pulse", "-i", "default", "-ac", "1", "-af", "aresample=async=1",
//                          "-c:a", "opus", "-strict", "-2", "-b:a", "192k", "-ar", "48000",
//                          "-c:v", "copy", "out.ogg"};
//    thread th2(&ffmpeg_main, 30, Mic2WavMod);
//////////// MAIN COMMANDS
//    char* RAWVideoPackets[36] = {"ffmpeg","-re", "-y","-f","v4l2","-framerate","25","-video_size","640x480","-i","/dev/video0","-pix_fmt", "yuv420p",
//                                        "-preset", "ultrafast", "-tune", "zerolatency", "-r", "10", "-b:v", "512k","-s", "640x480", "-c:v", "rawvideo", "-ac", "2",
//                                        "-ab", "32k", "-ar", "44100", "-vf", "format=yuv420p", "-update", "1", "output.yuv"};
//    thread th2(&ffmpeg_main, 36, RAWVideoPackets);
//
//    char *PreEncodedVideoPackets[36] = {"ffmpeg", "-re", "-y", "-f", "v4l2", "-framerate", "25", "-video_size",
//                                         "640x480", "-i", "/dev/video0", "-pix_fmt", "yuv420p",
//                                         "-preset", "ultrafast", "-tune", "zerolatency", "-r", "10", "-b:v", "512k",
//                                         "-s", "640x480", "-ac", "2", "-ab", "32k", "-ar", "44100", "-vf",
//                                         "format=yuv420p",
//                                         "-update", "1", "-flush_packets", "0", "output.yuv"};
//    thread th2(&ffmpeg_main, 36, PreEncodedVideoPackets);
//
//    char *PreEncodedAudioPackets[18] = {"ffmpeg", "-re", "-y",
//                         "-f", "pulse",
//                         "-channel_layout", "stereo",
//                         "-i", "default",
//                         "-c:a", "opus",
//                         "-strict", "-2",
//                         "-sample_rate", "48k",
//                         "-b:a", "32k",
//                         "out.ogg"};
//    thread th2(&ffmpeg_main, 18, PreEncodedAudioPackets);
    char *RAWAudioPackets[14] = {"ffmpeg", "-y", "-re", "-f", "pulse", "-i", "default", "-ac", "1",
                         "-sample_rate", "48000", "-b:a", "32000", "output_audio.wav"};
    thread th2(&ffmpeg_main, 14, RAWAudioPackets);
//    char* RAWAudioVideoPackets[33] = {
//            "ffmpeg","-re", "-y","-f","pulse",
//            "-i", "default", "-f","v4l2", "-i",
//            "/dev/video0", "-channle_layout", "2", "-pix_fmt", "yuv420p","-r","10",
//            "-b:v","512k","-s","640x480","-c:v",
//            "rawvideo", "-ac", "1","-sample_rate", "48000",
//            "-ab", "32000","-vf","format=yuv420p", "-update",
//            "1","testVideo.yuv","testAudio.wav"};
//
//    thread th2(&ffmpeg_main, 33, RAWAudioVideoPackets);
    AudioCapturer = new rtc::RefCountedObject<MyAudioCapturer>();
    AudioCapturer->Init();
    AudioCapturer->InitRecording();
    WebSocketEngine webSocketEngine;
    webSocketEngine.init();
    th2.join();
    fclose(outfile);
/////////////////////////
    return 0;
}
//#include <iostream>
//using namespace std;
//
//int main()
//{
//    typedef struct  WAV_HEADER
//    {
//        // RIFF Chunk
//        uint8_t         Chunk_ID[4];        // RIFF
//        uint32_t        Chunk_data_Size;      // RIFF Chunk data Size
//        uint8_t         RIFF_TYPE_ID[4];        // WAVE
//        // format sub-chunk
//        uint8_t         Chunk1_ID[4];         // fmt
//        uint32_t        Chunk1_data_Size;  // Size of the format chunk
//        uint16_t        Format_Tag;    //  format_Tag 1=PCM
//        uint16_t        Num_Channels;      //  1=Mono 2=Sterio
//        uint32_t        Sample_rate;  // Sampling Frequency in (44100)Hz
//        uint32_t        byte_rate;    // Byte rate
//        uint16_t        block_Align;     // 4
//        uint16_t        bits_per_sample;  // 16
//        /* "data" sub-chunk */
//        uint8_t         chunk2_ID[4]; // data
//        uint32_t        chunk2_data_Size;  // Size of the audio data
//    } obj;
//    obj header;
//    const char* filePath;
//    string input;
//    {
////        cout << "Enter the wave file name: ";
//        std::string input = "/home/webrtc-2/Desktop/webrtc-cpp-teamwork/out.ogg";
////        cin.get();
//        filePath = input.c_str();
//    }
//    FILE* fp = fopen("/home/webrtc-2/Desktop/webrtc-cpp-teamwork/out.ogg", "r");
//    if (fp == NULL)
//    {
//        fprintf(stderr, " file cannot be open %s \n", filePath);
//    }
//    {
//        // Read RIFF Chunk
//        fread(&header, 1, sizeof(header), fp);
//        cout << "Chunk ID                :" << header.Chunk_ID[0] << header.Chunk_ID[1] << header.Chunk_ID[2] << header.Chunk_ID[3] << endl;
//        cout << "Chunk data Size         :" << header.Chunk_data_Size << endl;
//        cout << "RIFF TYPE ID            :" << header.RIFF_TYPE_ID[0] << header.RIFF_TYPE_ID[1] << header.RIFF_TYPE_ID[2] << header.RIFF_TYPE_ID[3] << endl;
//        // format subchunk
//        cout<<"-------------------------------------------------------------------------------"<<endl;
//        cout << "Chunk1_ID               :" << header.Chunk1_ID[0] << header.Chunk1_ID[1] << header.Chunk1_ID[2] << header.Chunk1_ID[3] << endl;
//        cout << "Chunk1 data Size        :" << header.Chunk1_data_Size << endl;
//        cout << "Format Tag              :" << header.Format_Tag << endl;
//        cout << "Num_Channels            :" << header.Num_Channels << endl;
//        cout << "Sample_rate             :" << header.Sample_rate << endl;
//        cout << "byte_rate               :" << header.byte_rate << endl;
//        cout << "block_Align             :" << header.block_Align << endl;
//        cout << "bits per sample         :" << header.bits_per_sample << endl;
//        cout << "chunk2_ID               :" << header.chunk2_ID << endl;
//        cout << "chunk2_daata_size       :" << header.chunk2_data_Size << endl;
//
//        return 0;
//    }
//}