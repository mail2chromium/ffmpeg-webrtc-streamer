extern "C"
{
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
#include <libswresample\swresample.h>
}

#include <iostream>

using namespace std;
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

int main() {
    char infile[] = "E:\\test_audio.wav";
    char outfile[] = "PcmToAac.aac";

    FILE* testFile = fopen("E:\\test_PCMAAC.audio", "wb+");
    int framePtsCount = 0;

    //register muxer,demuters 格式
    av_register_all();
    //register codec
    avcodec_register_all();

    //1  打开音频编码器
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec) {
        cout << "avcodec_find_encoder error" << endl;
        getchar();
        return -1;
    }
    //编码器上下文
    AVCodecContext *c = avcodec_alloc_context3(codec);
    if (!c) {
        cout << "avcodec_alloc_context3 error" << endl;
        getchar();
        return -1;
    }
    //编码器参数设置
    c->bit_rate = 64000;//比特率  ：每秒传输的比特(bit)数,比特率越高，传输数据速度越快
    //采样率:每秒采集的样本数
    c->sample_rate = 44100;
    //样本格式和存储方式  float, planar
    c->sample_fmt = AV_SAMPLE_FMT_FLTP;
    //音频通道布局 先存放左声道，然后右声道
//    c->channel_layout = AV_CH_LAYOUT_STEREO;
    c->channel_layout = av_get_default_channel_layout(1);
    //通道数
    c->channels = 1;
    c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    //打开编码器
    int ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        cout << "avcodec_open2 error" << endl;
        getchar();
        return -1;
    }
    cout << "avcodec_open2 success!" << endl;

    //2 打开输出封装的上下文
    AVFormatContext *oc = NULL;
    avformat_alloc_output_context2(&oc, NULL, NULL, outfile);
    if (!oc) {
        cout << "avformat_alloc_output_context2 error" << endl;
        getchar();
        return -1;
    }

    //第二个参数为空，就是根据文件格式，自动选择合适的编码器
    AVStream *st = avformat_new_stream(oc, NULL);
    st->codecpar->codec_tag = 0;
    avcodec_parameters_from_context(st->codecpar, c);


    //3,open io,write head
    ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
    if (ret < 0) {
        cout << "avio_open error" << endl;
        getchar();
        return -1;
    }
    //写文件头
    ret = avformat_write_header(oc, NULL);

    //4 创建音频重采样上下文
    SwrContext *actx = NULL;
    actx = swr_alloc_set_opts(actx,
                              c->channel_layout, c->sample_fmt, c->sample_rate,//输出格式
                              /*AV_CH_LAYOUT_STEREO*/av_get_default_channel_layout(1), AV_SAMPLE_FMT_S16, 44100,//输入格式
                              0, 0);
    if (!actx) {
        cout << "swr_alloc_set_opts error" << endl;
        getchar();
        return -1;
    }
    ret = swr_init(actx);
    if (ret < 0) {
        cout << "swr_init error" << endl;
        getchar();
        return -1;
    }
    //5 打开输入音频文件，进行重采样
    AVFrame *frame = av_frame_alloc();
    frame->format = AV_SAMPLE_FMT_FLTP;
    frame->channels = 1;
    //音频通道布局
    frame->channel_layout = av_get_default_channel_layout(1);
    frame->nb_samples = 1024;//一帧音频存放的样本数量
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        cout << "av_frame_get_buffer error" << endl;
        getchar();
        return -1;
    }

    int readSize = frame->nb_samples * 2;
    char *pcm = new char[readSize];
    FILE *fp = fopen(infile, "rb");

    if (!fp) {
        cout << "open input file error" << endl;
        return -1;
    }

    //开始编码
    for (;;) {
        int len = fread(pcm, 1, readSize, fp);
        if (len <= 0) {
            cout << "fread break" << endl;
            break;
        }
        const uint8_t *data[1];
        data[0] = (uint8_t *) pcm;

        len = swr_convert(actx, frame->data, frame->nb_samples,
                          data, frame->nb_samples
        );
        if (len <= 0) {
            cout << "swr_convert break" << endl;
            break;
        }

        AVPacket pkt;
        av_init_packet(&pkt);

        frame->pts = framePtsCount;
        framePtsCount += frame->nb_samples;

        //6 音频编码
        ret = avcodec_send_frame(c, frame);
        if (ret != 0) continue;
        ret = avcodec_receive_packet(c, &pkt);
        if (ret != 0) continue;

        //7 音频封装成aac文件
//        pkt.stream_index = 0;
//        pkt.pts = 0;
//        pkt.dts = 0;

        if (testFile) {
            fwrite(pkt.data, sizeof(uint8_t),
                   static_cast<size_t>(pkt.size), testFile);
        }

        ret = av_interleaved_write_frame(oc, &pkt);
        cout << "[" << len << "]";
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    //6 音频编码
    ret = avcodec_send_frame(c, nullptr);
    while (ret >= 0) {
        ret = avcodec_receive_packet(c, &pkt);
        if (ret >= 0) {
            //7 音频封装成aac文件
            pkt.stream_index = 0;
//            pkt.pts = 0;
//            pkt.dts = 0;

//            if (testFile) {
//                fwrite(pkt.data, sizeof(uint8_t),
//                       static_cast<size_t>(pkt.size), testFile);
//            }

            ret = av_interleaved_write_frame(oc, &pkt);
            cout << "[flush]";
        }
    }

    if (testFile) {
        fclose(testFile);
        testFile = nullptr;
    }

    delete pcm;
    pcm = NULL;

    //写文件尾
    av_write_trailer(oc);

    //关闭输出流
    avio_close(oc->pb);
    //清理封装输出上下文
    avformat_free_context(oc);

    //关闭编码器
    avcodec_close(c);
    //清理编码器上下文
    avcodec_free_context(&c);

    cout << "======================end=========================" << endl;
    return 0;
}