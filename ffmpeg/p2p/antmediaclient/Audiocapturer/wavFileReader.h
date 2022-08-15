//
// Created by webrtc on 6/8/22.
//

#ifndef FFMPEG_PROJECT_WAVFILEREADER_H
#define FFMPEG_PROJECT_WAVFILEREADER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "MyAudioCapturer.h"

using namespace std;

//extern FILE *outfile;
extern "C" int writeAPI(uint8_t*, int);
static bool meta_written = false;

static FILE *outfile = fopen("raw_output.wav", "a+");

// WAVE PCM soundfile format (you can find more in https://ccrma.stanford.edu/courses/422/projects/WaveFormat/ )
typedef struct header_file {
    // RIFF Chunk
    uint8_t         Chunk_ID[4] = {'R', 'I', 'F', 'F'};        // RIFF
    uint32_t        Chunk_data_Size = 1037482;      // RIFF Chunk data Size
    uint8_t         RIFF_TYPE_ID[4] = {'W', 'A', 'V', 'E'};        // WAVE
    // format sub-chunk
    uint8_t         Chunk1_ID[4] = {'f', 'm', 't', ' '};         // fmt
    uint32_t        chunk1_data_Size = 16;  // Size of the format chunk
    uint16_t        Format_Tag = 1;    //  format_Tag 1=PCM
    uint16_t        Num_Channels = 1;      //  1=Mono 2=Sterio
    uint32_t        Sample_rate = 48000;  // Sampling Frequency in (44100)Hz
    uint32_t        byte_rate = 96000;    // Byte rate
    uint16_t        block_Align = 2;     // 4
    uint16_t        bits_Per_Sample = 16;  // 16
    /* "data" sub-chunk */
    uint8_t         chunk2_ID[4] = {'L', 'I', 'S', 'T'}; // data
    uint32_t        chunk2_data_Size = 26;  // Size of the audio data
} header;

typedef struct header_file *header_p;

class wavFileReader {
public:
    static wavFileReader* getinstance() {
        if(!instance)
            instance = new wavFileReader;
        return instance;
    }
//    void readFile(rtc::scoped_refptr<MyAudioCapturer> AudioCapturer);
    void writeFile(uint8_t*, int);
    static wavFileReader* instance;
};

#endif //FFMPEG_PROJECT_WAVFILEREADER_H
