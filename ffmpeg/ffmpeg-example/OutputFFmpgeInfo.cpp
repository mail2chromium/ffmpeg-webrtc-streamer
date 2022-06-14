//
// 输出ffmpeg相关信息
//

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;

const char *Setstr(const char *val) {
    return val;
}

#define AddSet(val) Setstr(#val)

static long long getCurrentTimeMills() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

string getTime() {
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return tmp;
}

int av_compare_ts_TEST(int64_t ts_a, AVRational tb_a, int64_t ts_b, AVRational tb_b) {
    int64_t a = tb_a.num * (int64_t) tb_b.den;
    int64_t b = tb_b.num * (int64_t) tb_a.den;
    //FFABS 表示获取绝对值
    if ((FFABS(ts_a) | a | FFABS(ts_b) | b) <= INT_MAX)
        return (ts_a * a > ts_b * b) - (ts_a * a < ts_b * b);
    if (av_rescale_rnd(ts_a, a, b, AV_ROUND_DOWN) < ts_b)
        return -1;
    if (av_rescale_rnd(ts_b, b, a, AV_ROUND_DOWN) < ts_a)
        return 1;
    return 0;
}

std::time_t getTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return timestamp;
}

int main(int argc, char **argv) {
//    char *info = (char *)malloc(40000);
//    memset(info, 0, 40000);
//
//    avcodec_register_all();
//
//    AVCodec *c_temp = av_codec_next(NULL);
//
//    while (c_temp != NULL)
//    {
//        if (c_temp->decode != NULL)
//        {
//            strcat(info, "[Decode]");
//        }
//        else
//        {
//            strcat(info, "[Encode]");
//        }
//        switch (c_temp->type)
//        {
//            case AVMEDIA_TYPE_VIDEO:
//                strcat(info, "[Video]");
//                break;
//            case AVMEDIA_TYPE_AUDIO:
//                strcat(info, "[Audeo]");
//                break;
//            default:
//                strcat(info, "[Other]");
//                break;
//        }
//        sprintf(info, "%s %10s\n", info, c_temp->name);
//        c_temp = c_temp->next;
//    }
//    puts(info);
//    free(info);

//    fprintf(stderr, "time is %lld",getCurrentTimeMills());
//
//    string  time = getTime();
//    cout << time << endl;
//
//    return 0;

//    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
//            std::chrono::system_clock::now().time_since_epoch()
//    );
//
//    long long time = ms.count();
//    std::cout << time << std::endl;
//    return 0;

//    av_compare_ts_TEST(100, AVRational {1, 25}, 200, AVRational {1, 44100});
//    string testStr = AddSet(AV_CODEC_ID_HAP);
//    printf("%s", testStr.c_str());
    printf("%lld", getTimeStamp());
    return 0;
}