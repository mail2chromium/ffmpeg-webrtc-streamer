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

// WAVE PCM soundfile format (you can find more in https://ccrma.stanford.edu/courses/422/projects/WaveFormat/ )
typedef struct header_file {
    char chunk_id[4];
    int chunk_size;
    char format[4];
    char subchunk1_id[4];
    int subchunk1_size;
    short int audio_format;
    short int num_channels;
    int sample_rate;            // sample_rate denotes the sampling rate.
    int byte_rate;
    short int block_align;
    short int bits_per_sample;
    char subchunk2_id[4];
    int subchunk2_size;            // subchunk2_size denotes the number of samples.
} header;

typedef struct header_file *header_p;

class wavFileReader {
public:
    void readFile(rtc::scoped_refptr<MyAudioCapturer> AudioCapturer);
};


#endif //FFMPEG_PROJECT_WAVFILEREADER_H
