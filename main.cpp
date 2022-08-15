//
// Created by webrtc-2 on 5/31/22.
//

#include "api/media_types.h"
#include "rtc_base/arraysize.h"
#include "p2p/antmediaclient/Engines/WebSocketEngine.h"
#include "p2p/antmediaclient/WebSocket/WebSocketHelper.h"
#include "p2p/antmediaclient/Capturer/vcm_capturer.h"
#include "p2p/antmediaclient/AntMedia/Constants.h"

extern "C" {
#include "fftools/ffmpeg.h"
}

#include <iostream>
#include <cstdlib>
#include <thread>

#include "p2p/antmediaclient/Audiocapturer/wavFileReader.h"

extern string local_web_url;
extern string local_stream_id;

static int f(char const **p, char const *q, size_t nmemb)
{
    for (size_t i = 0; i < nmemb; ++i)
        if (strstr(p[i], q) != NULL)
            return i;
    return 0;
}

typedef enum
{
    GET_DELIM_OK=0,
    GET_DELIM_NOT_FOUND_DELIM,
    GET_DELIM_PREFIX_TOO_LONG,
    GET_ID_PREFIX_TOO_LONG
} GET_DELIM_RESULT;

/**
 * Copy prefix of string pointed by pbuf before first occurance of delim
 * to dst. Increment pbuf so that it points to part of string right after
 * occurance of delim. Return 0 if operation succeeds, error code (to specify)
 * otherwise.
 * @param dst_size size of buffer dst
 */

static GET_DELIM_RESULT split_(const char** pbuf, const char* delim, char* dst, char* id, int dst_size, int id_size)
{
    const char* sub = strstr(*pbuf,delim);
    if(sub==NULL)
        return GET_DELIM_NOT_FOUND_DELIM;
    else
    {
        int prefix_len = sub-(*pbuf);
        if(prefix_len+1>dst_size)
            return GET_DELIM_PREFIX_TOO_LONG;
        // strncat requires dst to be null-terminated
        *dst=0;
        strncat(dst,*pbuf,prefix_len);
        *pbuf+=prefix_len+strlen(delim);

        *id=0;
        int id_length = strlen(*pbuf)-(prefix_len+strlen(delim));
        if(id_length+1>id_size)
            return GET_ID_PREFIX_TOO_LONG;

        strncat(id,*pbuf,id_length);
        *pbuf+=id_length;

        return GET_DELIM_OK;
    }
}

/////////// For RAW Video ///////////////////
//    int argc = 36;
//    char* argv[36] = {"ffmpeg","-re", "-y","-f","v4l2","-framerate","25","-video_size","640x480","-i","/dev/video0","-pix_fmt", "yuv420p",
//                                        "-preset", "ultrafast", "-tune", "zerolatency", "-r", "10", "-b:v", "512k","-s", "640x480", "-c:v", "rawvideo", "-ac", "2",
//                                        "-ab", "32k", "-ar", "44100", "-vf", "format=yuv420p", "-update", "1", "output.yuv"};

/////////// For Pre-Encoded Video ///////////////////
//    int argc = 36;
//    char *argv[36] = {
//                    "ffmpeg", "-re", "-y", "-f", "v4l2", "-framerate", "60", "-video_size",
//                    "640x480", "-i", "/dev/video0", "-pix_fmt", "yuv420p",
//                    "-preset", "ultrafast", "-tune", "zerolatency", "-r", "10", "-b:v", "1024k",
//                    "-ac", "2", "-ab", "32k", "-ar", "44100", "-vf",
//                    "format=yuv420p",
//                    "-update", "1", "-flush_packets", "0", "-vcodec", "h264", "output.yuv"
//                    };

/////////// For Pre-Encoded Audio ///////////////////
//    int argc = 18;
//    char *argv[18] = {"ffmpeg", "-re", "-y",
//                         "-f", "pulse",
//                         "-channel_layout", "stereo",
//                         "-i", "default",
//                         "-c:a", "opus",
//                         "-strict", "-2",
//                         "-sample_rate", "48k",
//                         "-b:a", "32k",
//                         "out.ogg"};

/////////// For RAW Audio ///////////////////
//int argc = 14;
//char *argv[14] = {"ffmpeg", "-y", "-re", "-f", "pulse", "-i", "default", "-ac", "1",
//                             "-sample_rate", "48000", "-b:a", "32000", "output_audio.wav"};

/////////// For RAW Audio & Video ///////////////////
//    int argc = 33;
//    char* argv[33] = {
//            "ffmpeg","-re", "-y","-f","pulse",
//            "-i", "default", "-f","v4l2", "-i",
//            "/dev/video0", "-channle_layout", "2", "-pix_fmt", "yuv420p","-r","10",
//            "-b:v","512k","-s","640x480","-c:v",
//            "rawvideo", "-ac", "1","-sample_rate", "48000",
//            "-ab", "32000","-vf","format=yuv420p", "-update",
//            "1","testVideo.yuv","testAudio.wav"};

/////////// For RAW Audio Publishing through AntMedia ///////////////////
//    int argc = 16;
//    char *argv[16] = {
//            "ffmpeg", "-y", "-re", "-f", "pulse", "-i",
//            "default", "-ac", "1",
//            "-sample_rate", "48000", "-b:a", "32000",
//            "-f", "mpegts",
//            "ws://localhost:5080/WebRTCAppEE/websocket/id=test_id"
//    };

/////// For Pre-Encoded Audio Publishing through AntMedia ///////////////////
//    int argc = 20;
//    char *argv[20] = {"ffmpeg", "-re", "-y",
//                         "-f", "pulse",
//                         "-channel_layout", "stereo",
//                         "-i", "default",
//                         "-c:a", "opus",
//                         "-strict", "-2",
//                         "-sample_rate", "48k",
//                         "-b:a", "32k",
//                         "-f", "mpegts",
//                         "ws://localhost:5080/WebRTCAppEE/websocket/id=OpusAudioLocal"
//                    };

/////////// For Pre-Encoded Video Publishing through AntMedia ///////////////////
//    int argc = 38;
//    char* argv[38] = {
//            "ffmpeg",                                                               //program name
//            "-re", "-y", "-f", "v4l2",                                              //input properties
//            "-framerate", "60", "-video_size", "640x480",                           //input properties
//            "-i", "/dev/video0",                                                    //input device
//            "-pix_fmt", "yuv420p", "-preset", "ultrafast", "-tune", "zerolatency",  //output properties
//            "-r", "10", "-b:v", "512k", "-ac", "2", "-ab", "96k", "-ar", "44100",  //output properties
//            "-vf", "format=yuv420p", "-update", "1", "-flush_packets", "0",         //output properties
//            "-vcodec", "h264", "-f", "mpegts",                                      //output properties
//            "ws://localhost:5080/WebRTCAppEE/websocket/id=mystream"                 //output device
//    };

///////// For Raw Video Publishing through AntMedia ///////////////////
//    int argc = 38;
//    char* argv[38] = {
//            "ffmpeg","-re", "-y","-f","v4l2","-framerate","25",
//            "-video_size","640x480","-i","/dev/video0","-pix_fmt", "yuv420p",
//            "-preset", "ultrafast", "-tune", "zerolatency", "-r", "10",
//            "-b:v", "512k","-s", "640x480", "-c:v", "rawvideo", "-ac", "2",
//            "-ab", "32k", "-ar", "44100", "-vf", "format=yuv420p", "-update", "1",
//            "-f", "mpegts",                                                         //output properties
//            "ws://localhost:5080/WebRTCAppEE/websocket/id=rtp_stream"          //output device
//    };

///////// For Raw Audio and Video Publishing through AntMedia ///////////////////
//    int argc = 39;
//    char* argv[39] = {
//                        "ffmpeg","-re", "-y","-f","pulse",
//                        "-i", "default", "-channel_layout", "stereo",
//                        "-f","v4l2", "-i",
//                        "/dev/video0", "-pix_fmt", "yuv420p","-r","10",
//                        "-b:v","512k","-s","640x480","-c:v",
//                        "rawvideo", "-ac", "1","-sample_rate", "48000", "-b:a", "32000",
//                        "-ab", "32000","-input_format","yuv420p",
//                        "-update", "1", "-preset", "ultrafast",
//                        "a.yuv","b.wav"
////                        "-f", "mpegts",
////                        "ws://localhost:5080/WebRTCAppEE/websocket/id=stream_local"
//                    };

///////// For Pre-Encoded Audio and Video Publishing through AntMedia ///////////////////
//    int argc = 38;
//    char* argv[38] = {
//                        "ffmpeg","-re", "-y","-f","pulse",
//                        "-i", "default", "-channel_layout", "stereo",
//                        "-f","v4l2", "-i",
//                        "/dev/video0", "-pix_fmt", "yuv420p","-r","10",
//                        "-b:v","512k","-s","640x480","-c:v",
//                        "rawvideo", "-ac", "1","-sample_rate", "48000", "-b:a", "32000",
//                        "-ab", "32000","-vf","format=yuv420p", "-update",
//                        "1", "-f", "mpegts",
//                        "ws://localhost:5080/WebRTCAppEE/websocket/id=stream_local"
//                    };

int main(int argc, char** argv) {

    /*TODO; check if filename constains 'ws' prefix, if so then
       * split the filename into websocket url and stream id*/
    char filename_[100];
    char streamID[100];

    int indx = f((const char**)(argv), "ws:", argc);

    if(indx) {
        //'ws' prefix is set in filename
        av_log(NULL, AV_LOG_INFO, "Requested to stream via websocket %s\n", argv[indx]);

        const char *pbuf = argv[indx];
        int rc, ret;

        rc = split_(&pbuf, "/id=", filename_, streamID, 100, 100);

        if(rc == GET_DELIM_OK) {
            RTC_LOG(LS_INFO) << "given websocket url contains URL=" << filename_ << " and StreamID=" << streamID;
            string url_(filename_), id_(streamID);
            local_web_url = url_;
            local_stream_id = id_;
        }
        else {
            RTC_LOG(LS_INFO) << "got error, revisit command" << argv[indx];
            return 0;
        }

    }
    else {
        local_web_url = ANT_MEDIA_SERVER_URL_LOCAL;
        local_stream_id = LOCAL_STREAM_ID;
    }

    int IsBitRate4VideoSet = f((const char**)(argv), "-b:v", argc);
    int IsBitRate4AudioSet = f((const char**)(argv), "-b:a", argc);
    int IsCodec4VideoSet   = f((const char**)(argv), "-vcodec", argc);
    int IsRaw4VideoSet     = f((const char**)(argv), "-c:v", argc);
    int IsCodec4AudioSet   = f((const char**)(argv), "-strict", argc);

    if(!IsRaw4VideoSet)
        WebSocketHelper::raw_data = 0;
    else
        WebSocketHelper::raw_data = 1;

    if(IsCodec4VideoSet)
        VideoCodecIsRequested = true;

    if(IsCodec4AudioSet)
        AudioCodeIsRequested = true;

    if(IsBitRate4AudioSet)
        RequestedAudioTrack = true;

    if(IsBitRate4VideoSet)
        RequestedVideoTrack = true;

    if(IsBitRate4AudioSet) {
//        AudioCapturer = new rtc::RefCountedObject<MyAudioCapturer>();
//        AudioCapturer->Init();
//        AudioCapturer->InitRecording();
    }

    thread th2(&ffmpeg_main, argc, argv);

    WebSocketEngine::getInstance()->init(local_web_url, local_stream_id);

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







