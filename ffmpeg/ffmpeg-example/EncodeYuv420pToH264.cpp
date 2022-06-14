/**
 * 转换yuv420p为h264
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FFmpegCompat.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile) {
    int ret;

    /* send the frame to the encoder */
    if (frame) {
        printf("Send frame %3" PRId64"\n", frame->pts);
    }

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN)) {
            fprintf(stderr, "Error EAGAIN\n");
            return;
        } else if (ret == AVERROR_EOF) {
            fprintf(stderr, "Error AVERROR_EOF\n");
            return;
        } else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3" PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

int main(int argc, char **argv) {
    const char *outFileName, *inputFileName;
    const AVCodec *codec;
    AVCodecContext *c = nullptr;
    int ret;
    FILE *outFile = nullptr, *inputFile = nullptr;
    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

//    int inputWidth = 720;
//    int inputHeight = 1280;
    int inputWidth = 352;
    int inputHeight = 288;

    if (argc <= 2) {
        fprintf(stderr, "Description : change yuv420p to h264\n");
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }
    inputFileName = argv[1];
    outFileName = argv[2];

    inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        fprintf(stderr, "open error: %s", inputFileName);
        exit(0);
    }

    outFile = fopen(outFileName, "wb+");
    if (!outFile) {
        fprintf(stderr, "open error: %s", outFileName);
        exit(0);
    }

    avcodec_register_all();

    const char *codec_name = "libx264";

    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = inputWidth;
    c->height = inputHeight;
    /* frames per second */
    c->time_base = (AVRational) {1, 25};
    c->framerate = (AVRational) {25, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec: %s\n", av_err2str_ffmpeg(ret));
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 8);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

//    frame = av_frame_alloc();
//    frame->width = c->width;
//    frame->height = c->height;
//    frame->format = c->pix_fmt;
//    // ffmpeg4.0
//    int size = av_image_get_buffer_size(c->pix_fmt, c->width, c->height, 1);
//    uint8_t *picture_buf = (uint8_t *)av_malloc(size);
//    av_image_fill_arrays(frame->data, frame->linesize, picture_buf, AV_PIX_FMT_YUV420P, c->width, c->height, 1);

    size_t readSize = 0;
    int frameCount = 0;
    int y_size = inputWidth * inputHeight;

    while (true) {

        fflush(stdout);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0) {
            fprintf(stderr, "frame can not write\n");
            exit(1);
        }


        //读一帧数据出来
        readSize = fread(frame->data[0], 1, y_size, inputFile);
        if (readSize == 0) {
            fprintf(stdout, "end of file\n");
            break;
        }
        readSize = fread(frame->data[1], 1, y_size / 4, inputFile);
        readSize = fread(frame->data[2], 1, y_size / 4, inputFile);

//        if (fread(picture_buf, 1, y_size * 3 / 2, inputFile) <= 0) {
//            fprintf(stderr, "\"read file fail!\"\n");
//            return -1;
//        } else if (feof(inputFile)) {
//            fprintf(stdout, "end of file\n");
//            break;
//        }
//        frame->data[0] = picture_buf; // Y
//        frame->data[1] = picture_buf + y_size; // U
//        frame->data[2] = picture_buf + y_size * 5 / 4;// V

        frame->pts = frameCount;

        /* encode the image */
        encode(c, frame, pkt, outFile);

        frameCount++;
    }

    /* flush the encoder */
    encode(c, NULL, pkt, outFile);

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), outFile);
    fclose(outFile);

    fclose(inputFile);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}

