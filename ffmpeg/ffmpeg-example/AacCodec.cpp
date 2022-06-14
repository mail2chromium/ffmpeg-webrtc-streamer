#include <stdio.h>
#include "AacCodec.h"
#include "Log.h"

#undef LOG_TAG
#define LOG_TAG "AacCodec"

#define DUMP_DEBUG true
#define ADTS_HEADER_LENGTH 7

AacCodec::AacCodec()
	:mAVCodec(NULL),
	 mAVCodecContext(NULL),
	 mAVFrame(NULL),
	 mBufferSize(0),
	 mEncoderData(NULL),
	 outFile("/storage/emulated/0/test.aac"),
	 mAVFormatContext(NULL),
	 mAVStream(NULL),
	 mADTSFile(NULL)
{
	if(DUMP_DEBUG){
		mADTSFile = fopen("/storage/emulated/0/adts.aac", "wb+");
		mPCMFile  = fopen("/storage/emulated/0/raw_audio.pcm", "wb+");
	}
}
int AacCodec::start(){
	ALOGW("start");
	av_register_all();
	//avcodec_register_all();
	mAVCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);//查找AAC编码器
	if(!mAVCodec){
		ALOGE("encoder AV_CODEC_ID_AAC not found");
		return -1;
	}
	mAVCodecContext = avcodec_alloc_context3(mAVCodec);
	if(mAVCodecContext != NULL){
		mAVCodecContext->codec_id         = AV_CODEC_ID_AAC;
		mAVCodecContext->codec_type       = AVMEDIA_TYPE_AUDIO;
		mAVCodecContext->bit_rate         = 12200;
		mAVCodecContext->sample_fmt       = AV_SAMPLE_FMT_FLTP;
		mAVCodecContext->sample_rate      = 8000;
		mAVCodecContext->channel_layout   = AV_CH_LAYOUT_MONO;
		mAVCodecContext->channels         = av_get_channel_layout_nb_channels(mAVCodecContext->channel_layout);
	}else {
		ALOGE("avcodec_alloc_context3 fail");
		return -1;
	}
	ALOGW("start  3 channels %d",mAVCodecContext->channels);
	if(avcodec_open2(mAVCodecContext, mAVCodec, NULL) < 0){
		ALOGE("aac avcodec open fail");
		av_free(mAVCodecContext);
		mAVCodecContext = NULL;
		return -1;
	}
	mAVFrame = av_frame_alloc();
	if(!mAVFrame) {
		avcodec_close(mAVCodecContext);
		av_free(mAVCodecContext);
		mAVCodecContext = NULL;
        ALOGE("av_frame_alloc fail");
		return -1;
	}
	mAVFrame->nb_samples = mAVCodecContext->frame_size;
	mAVFrame->format = mAVCodecContext->sample_fmt;
	mAVFrame->channel_layout = mAVCodecContext->channel_layout;

	mBufferSize = av_samples_get_buffer_size(NULL, mAVCodecContext->channels, mAVCodecContext->frame_size, mAVCodecContext->sample_fmt, 0);
	if(mBufferSize < 0){
		ALOGE("av_samples_get_buffer_size fail");
		av_frame_free(&mAVFrame);
		mAVFrame = NULL;
		avcodec_close(mAVCodecContext);
		av_free(mAVCodecContext);
		mAVCodecContext = NULL;
        ALOGE("mBufferSize fail");
		return -1;
	}
	mEncoderData = (uint8_t *)av_malloc(mBufferSize);

	if(!mEncoderData){
		ALOGE("av_malloc fail");
		av_frame_free(&mAVFrame);
		mAVFrame = NULL;
		avcodec_close(mAVCodecContext);
		av_free(mAVCodecContext);
		mAVCodecContext = NULL;
        ALOGE("mEncoderData fail");
		return -1;
	}

	avcodec_fill_audio_frame(mAVFrame, mAVCodecContext->channels, mAVCodecContext->sample_fmt, (const uint8_t*)mEncoderData, mBufferSize, 0);

	if(DUMP_DEBUG){//存AAC原始音频数据
        
        ALOGE("99999999999999999999999999\n");
        
        
		mAVFormatContext = avformat_alloc_context();
		mAVOUtputFormat = av_guess_format(NULL, outFile, NULL);
		mAVFormatContext->oformat = mAVOUtputFormat;

		//Open output URL
		if (avio_open(&mAVFormatContext->pb, outFile, AVIO_FLAG_READ_WRITE) < 0){
			ALOGE("Failed to open output file!\n");
			return -1;
		}
		mAVStream = avformat_new_stream(mAVFormatContext, 0);
		if (!mAVStream){
            ALOGE("mAVStream fail");
			return -1;
		}
		av_dump_format(mAVFormatContext, 0, outFile, 1);
		//Write Header
		avformat_write_header(mAVFormatContext, NULL);
		}
    
    ALOGE("hhhhhhhhhhhhhhhhhhhhhhhhhh  start.");
	return 0;
}

int AacCodec::encode_pcm_data(void* pIn, int frameSize){
	int encode_ret = -1;
	int got_packet_ptr = 0;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;
    
  
	

    ALOGW("start to encode_pcm_data..., frameSize is:%d", frameSize);
	if(mAVCodecContext && mAVFrame){
        
        ALOGW("333333333333333");
		short2float((int16_t *)pIn, mEncoderData, frameSize/2);
		  fwrite(mEncoderData, 1, frameSize*2 ,mPCMFile);
        mAVFrame->data[0] = mEncoderData;
		mAVFrame->pts = 0;
		//音频编码
		encode_ret = avcodec_encode_audio2(mAVCodecContext, &pkt, mAVFrame, &got_packet_ptr);
		ALOGW("encode_pcm_data ----encode_ret:%d,got_packet_ptr:%d",encode_ret,got_packet_ptr);
		if(encode_ret < 0){
			ALOGE("Failed to encode!\n");
			return encode_ret;
		}
		ALOGW("encode_pcm_data ----size =%d",pkt.size);
		pkt.stream_index = mAVStream->index;
		void *adts;
		if(DUMP_DEBUG && pkt.size > 0){
			if(mADTSFile){
				void *adts = malloc(ADTS_HEADER_LENGTH);
				//添加adts header 可以正常播放。
				addADTSheader((uint8_t *)adts, pkt.size+ADTS_HEADER_LENGTH);
				fwrite(adts, 1, ADTS_HEADER_LENGTH, mADTSFile);
				fwrite(pkt.data, 1, pkt.size, mADTSFile);
				free(adts);
			}
			//无adts header，aac原始数据，不能播放
			mAVOUtputFormat->write_packet(mAVFormatContext, &pkt);
		}
		av_free_packet(&pkt);
	}
    
    ALOGW("out of encode_pcm_data...");
	return encode_ret;
}

void AacCodec::stop(){
	if(DUMP_DEBUG){
		//Write Trailer
		av_write_trailer(mAVFormatContext);
		avio_close(mAVFormatContext->pb);
		avformat_free_context(mAVFormatContext);

		fclose(mADTSFile);
		fclose(mPCMFile);
	}
	if(mAVFrame){
		av_frame_free(&mAVFrame);
		mAVFrame = NULL;
	}
	if(mEncoderData){
		av_freep(&mEncoderData);
		mEncoderData = NULL;
	}
	if(mAVCodecContext){
		avcodec_close(mAVCodecContext);
		av_free(mAVCodecContext);
		mAVCodecContext = NULL;
	}
}
void AacCodec::short2float(short* in, void* out, int len){
		register int i;
	for(i = 0; i < len; i++)
		((float*)out)[i] = ((float)(in[i])) / 32767.0;
}

void AacCodec::addADTSheader(uint8_t * in, int packet_size){
	int sampling_frequency_index = 11; //采样率下标
	int channel_configuration = mAVCodecContext->channels; //声道数
	in[0] = 0xFF;
	in[1] = 0xF9;
	in[2] = 0x40 | (sampling_frequency_index << 2) | (channel_configuration >> 2);//0x6c;
	in[3] = (channel_configuration & 0x3) << 6;
	in[3] |= (packet_size & 0x1800) >> 11;
	in[4] = (packet_size & 0x1FF8) >> 3;
	in[5] = ((((unsigned char)packet_size) & 0x07) << 5) | (0xff >> 3);
	in[6] = 0xFC;
}
