#ifndef AACCODEC_H_
#define AACCODEC_H_
#ifdef __cplusplus
extern "C"{
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"

class AacCodec {
public:
	AacCodec();
	int start();
	int encode_pcm_data(void* pIn, int frameSize);
	void stop();

private:
	AVCodec *mAVCodec;

	AVCodecContext *mAVCodecContext;
	AVFrame *mAVFrame;
	int mBufferSize;
	uint8_t *mEncoderData;

	const char *outFile;
	AVFormatContext *mAVFormatContext;
	AVStream *mAVStream;
	AVOutputFormat* mAVOUtputFormat;

	FILE *mADTSFile;
	FILE *mPCMFile;
	void short2float(short* in, void* out, int len);
	void addADTSheader(uint8_t * in, int packet_size);
};
#ifdef __cplusplus
}
#endif
#endif /* AACCODEC_H_ */
