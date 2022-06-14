//
// Created by mochangsheng on 2019/6/25.
// https://blog.csdn.net/rootusers/article/details/42551935
//https://blog.csdn.net/huangyifei_1111/article/details/46955021
//
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}
#define BUF_SIZE 4096*500

FILE *file;

//实现read_packet函数，从文件中读取模拟的是从内存中获取，同样可以实现为接收网络流
int read_packet(void *opaque, uint8_t *buf, int buf_size) {
    int n = 0;
    if (!feof(file)) {
        n = fread(buf, 1, buf_size, file);
        return n;
    } else
        return -1;
}

int main(int argc, char **argv) {
    file = fopen("cuc_ieschool.mp4", "rb");
    if (file == NULL) {
        fprintf(stderr, "open file error\n");
        return -1;
    }
    av_register_all();
    AVIOContext *pb = NULL;
    AVInputFormat *piFmt = NULL;
    AVInputFormat *pFmt = NULL;

    uint8_t *buf = (uint8_t *) av_mallocz(sizeof(uint8_t) * BUF_SIZE);

    pb = avio_alloc_context(buf, BUF_SIZE, 0, NULL, read_packet, NULL, NULL);
    if (av_probe_input_buffer(pb, &piFmt, "", NULL, 0, 0) < 0) { //探测从内存中获取到的媒体流的格式
        fprintf(stderr, "probe format failed\n");
        return -1;
    } else {
        fprintf(stdout, "format:%s[%s]\n", piFmt->name, piFmt->long_name);
    }
    return 0;
}
