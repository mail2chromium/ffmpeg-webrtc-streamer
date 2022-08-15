//
// Created by webrtc on 6/8/22.
//

#include "wavFileReader.h"

wavFileReader *wavFileReader::instance = 0;

//void wavFileReader::readFile(rtc::scoped_refptr<MyAudioCapturer> AudioCapturer) {
//    FILE *infile = fopen("input.wav", "rb");        // Open wave file in read mode
//    FILE *outfile = fopen("Output.wav", "wb");
//
//    // Create output ( wave format) file in write mode
//
//
//    const int BUFSIZE = 65535;                    // BUFSIZE can be changed according to the frame size required (eg:512)
//    int count = 0;                        // For counting number of frames in wave file.
//    short int buff16[BUFSIZE];                // short int used for 16 bit as input data format is 16 bit PCM audio
//    header_p meta = (header_p) malloc(
//            sizeof(header));    // header_p points to a header struct that contains the wave file metadata fields
//    int nb;                            // variable storing number of byes returned
//    if (infile) {
//        fread(meta, 1, sizeof(header), infile);
//        fwrite(meta, 1, sizeof(*meta), outfile);
//
////        cout << " Size of Header file is " << sizeof(*meta) << " bytes" << endl;
////        cout << " Sampling rate of the input wave file is " << meta->sample_rate << " Hz" << endl;
////        cout << " Number of samples in wave file are " << meta->subchunk2_size << " samples" << endl;
//
//
//        while (!feof(infile)) {
//            nb = fread(buff16, 1, BUFSIZE, infile);        // Reading data in chunks of BUFSIZE
//            cout << nb << endl;
//            count++;                    // Incrementing Number of frames
//
////            AudioCapturer->ProcessPacket(&buff16, BUFSIZE);
//            /* Insert your processing code here*/
//            std::string enc_str;
//            for (int j_ = 0; j_ < BUFSIZE; j_++) {
//                enc_str += std::to_string(buff16[j_]);
//                if (j_ < BUFSIZE - 1)
//                    enc_str += " ";
//            }
//            RTC_LOG(LS_ERROR) << "input.wav: " << BUFSIZE << " bytes: " << enc_str.c_str();
//            /* Insert your processing code here*/
//
//            fwrite(buff16, 1, nb, outfile);            // Writing read data into output file
//        }
//        cout << " Number of frames in the input wave file are " << count << endl;
//        fclose(infile);
//        fclose(outfile);
//    } else {
//        cout << "File not found\n";
//
//    }
//}

constexpr int16_t combine(std::uint8_t a, std::uint8_t b) {
    int16_t out = a << 8 | b;
    return out;
}

void wavFileReader::writeFile(uint8_t *bytes_data, int bytes_size) {
//    //TODO; convert uint8_t array to short
//    int i = 0;
//    int16_t *out;
//    if (bytes_size % 2 == 0) {
//        out = new int16_t [bytes_size / 2];
//    } else {
//        out = new int16_t [bytes_size / 2 + 1];
//    }
//    int j = 0;
//    for (; i < bytes_size; i += 2) {
//        if (i == bytes_size - 1) {
//            if (bytes_size % 2 == 0)
//                out[j] = combine(bytes_data[i], bytes_data[i + 1]);
//            else
//                out[j] = combine(bytes_data[i], 0);
//        } else
//            out[j] = combine(bytes_data[i], bytes_data[i + 1]);
//
//        j++;
//    }

    if (outfile) {
        if (!meta_written) {
            header_p meta = (header_p) malloc(
                    sizeof(header));    // header_p points to a header struct that contains the wave file metadata fields
            meta->Chunk_ID[0] = 'R';        // RIFF
            meta->Chunk_ID[1] = 'I';        // RIFF
            meta->Chunk_ID[2] = 'F';        // RIFF
            meta->Chunk_ID[3] = 'F';        // RIFF
            meta->Chunk_data_Size = 1037482;      // RIFF Chunk data Size
            meta->RIFF_TYPE_ID[0] = 'W';        // WAVE
            meta->RIFF_TYPE_ID[1] = 'A';        // WAVE
            meta->RIFF_TYPE_ID[2] = 'V';        // WAVE
            meta->RIFF_TYPE_ID[3] = 'E';        // WAVE
            // format sub-chunk
            meta->Chunk1_ID[0] = 'f';         // fmt
            meta->Chunk1_ID[1] = 'm';         // fmt
            meta->Chunk1_ID[2] = 't';         // fmt
            meta->Chunk1_ID[3] = ' ';         // fmt
            meta->chunk1_data_Size = 16;  // Size of the format chunk
            meta->Format_Tag = 1;    //  format_Tag 1=PCM
            meta->Num_Channels = 1;      //  1=Mono 2=Sterio
            meta->Sample_rate = 48000;  // Sampling Frequency in (44100)Hz
            meta->byte_rate = 96000;    // Byte rate
            meta->block_Align = 2;     // 4
            meta->bits_Per_Sample = 64;  // 16
            /* "data" sub-chunk */
            meta->chunk2_ID[0] = 'L'; // data
            meta->chunk2_ID[1] = 'I'; // data
            meta->chunk2_ID[2] = 'S'; // data
            meta->chunk2_ID[3] = 'T'; // data
            meta->chunk2_data_Size = 26;  // Size of the audio data

            fwrite(meta, 1, sizeof(*meta), outfile);
//            cout << " Size of Header file is " << sizeof(*meta) << " bytes" << endl;
//            cout << " Sampling rate of the input wave file is " << meta->sample_rate << " Hz" << endl;
//            cout << " Number of samples in wave file are " << meta->subchunk2_size << " samples" << endl;

            meta_written = true;
        }
//        /* Insert your processing code here*/
//        std::string enc_str;
//        for (int j_ = 0; j_ < j - 1; j_++) {
//            enc_str += std::to_string(out[j_]);
//            if (j_ < j - 2)
//                enc_str += " ";
//        }
//        RTC_LOG(LS_ERROR) << "audio_encoder_opus.cc status is " << j - 1 << " bytes: " << enc_str.c_str();
        fwrite(bytes_data, 1,  bytes_size, outfile);            // Writing read data into output file
    }

//    delete out;

}

int writeAPI(uint8_t *bytes_data, int size) {
    wavFileReader::getinstance()->writeFile(bytes_data, size);
    return 0;
}