//
// Created by mochangsheng on 2019/9/4.
//



#ifdef __cplusplus
extern "C"
{
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libavutil/opt.h>
#include <libavutil/avstring.h>
#include <libavutil/imgutils.h>
#include <libavutil/avassert.h>
#include <libavcodec/internal.h>

#ifdef __cplusplus
};
#endif

AVCodecID AV_CODEC_ID_MKVIDEO = AV_CODEC_ID_H264;

static av_cold int mk_encode_init(AVCodecContext *avctx) {
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(avctx->pix_fmt);

    avctx->coded_frame = av_frame_alloc();
    avctx->coded_frame->pict_type = AV_PICTURE_TYPE_I;
    avctx->bits_per_coded_sample = av_get_bits_per_pixel(desc);
    if (!avctx->codec_tag)
        avctx->codec_tag = avcodec_pix_fmt_to_codec_tag(avctx->pix_fmt);
    return 0;
}

static int mk_encode(AVCodecContext *avctx, AVPacket *pkt,
                     const AVFrame *frame, int *got_packet) {
    int ret = avpicture_get_size(avctx->pix_fmt, avctx->width, avctx->height);

    if (ret < 0)
        return ret;

    if (pkt->data == NULL && pkt->size == 0) {
        av_new_packet(pkt, ret);
        pkt->size = ret;
    }

// 	if ((ret = ff_alloc_packet2(avctx, pkt, ret)) < 0)
// 		return ret;

    if ((ret = avpicture_layout((const AVPicture *) frame, avctx->pix_fmt, avctx->width,
                                avctx->height, pkt->data, pkt->size)) < 0)
        return ret;

// 	if(avctx->codec_tag == AV_RL32("yuv2") && ret > 0 &&
// 		avctx->pix_fmt   == AV_PIX_FMT_YUYV422)
// 	{
// 			int x;
// 			for(x = 1; x < avctx->height*avctx->width*2; x += 2)
// 				pkt->data[x] ^= 0x80;
// 	}
    pkt->flags |= AV_PKT_FLAG_KEY;
    *got_packet = 1;
    return 0;
}

static av_cold int mk_close(AVCodecContext *avctx) {
    av_frame_free(&avctx->coded_frame);
    return 0;
}

AVCodec ff_mkvideo_encoder = {
        .name           = "mkvideo",
        .long_name      = "mk video",
        .type           = AVMEDIA_TYPE_VIDEO,
        .id             = AV_CODEC_ID_MKVIDEO,
        .capabilities    = 0,
        .supported_framerates = NULL,
        .pix_fmts        = NULL,
        .supported_samplerates = NULL,
        .sample_fmts    = NULL,
        .channel_layouts = NULL,
        .max_lowres        = 0,
        .priv_class        = NULL,
        .profiles        = NULL,
        .priv_data_size    = 0,
        .next            = NULL,
        .init_thread_copy = NULL,
        .update_thread_context = NULL,
        .defaults        = NULL,
        .init_static_data = NULL,
        .init           = mk_encode_init,
        .encode_sub        = NULL,
        .encode2        = mk_encode,
        .decode            = NULL,
        .close          = mk_close,
};

void help() {
    printf("**********************************************\n");
    printf("Usage:\n");
    printf("    MyMuxer [inputfile] [outputfile] \n");
    printf("\n");
    printf("Examples: \n");
    printf("    MyMuxer a.avi a.yuv \n");
    printf("**********************************************\n");
}

int main(int argc, char **argv) {
    if (argc < 3 || (!strcmp(argv[1], "--help"))) {
        help();
        return 0;
    }

    av_register_all();
    avcodec_register(&ff_mkvideo_encoder);

    AVFormatContext *in_fxt = NULL, *out_fxt = NULL;
    AVStream *out_stream = NULL;
    int video_index = -1;

    if (avformat_open_input(&in_fxt, argv[1], NULL, NULL) < 0) {
        printf("can not open the input file context!\n");
        goto end;
    }
    if (avformat_find_stream_info(in_fxt, NULL) < 0) {
        printf("can not find the stream info!\n");
        goto end;
    }

    if (avformat_alloc_output_context2(&out_fxt, NULL, NULL, argv[2]) < 0) {
        printf("can not alloc output context!\n");
        goto end;
    }

    for (int i = 0; i < in_fxt->nb_streams; i++) {
        if (in_fxt->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            //open decoder
            if (0 > avcodec_open2(in_fxt->streams[i]->codec, avcodec_find_decoder(in_fxt->streams[i]->codec->codec_id),
                                  NULL)) {
                printf("can not find or open decoder!\n");
                goto end;
            }
            video_index = i;
            //new stream
            out_stream = avformat_new_stream(out_fxt, NULL);
            if (!out_stream) {
                printf("can not new stream for output!\n");
                goto end;
            }
            //set codec context param
            out_stream->codec->codec = avcodec_find_encoder_by_name("mkvideo");
//            out_stream->codec->codec = avcodec_find_encoder(/*out_fxt->oformat->video_codec*/AV_CODEC_ID_MKVIDEO);
            out_stream->codec->height = in_fxt->streams[i]->codec->height;
            out_stream->codec->width = in_fxt->streams[i]->codec->width;

            out_stream->codec->time_base = in_fxt->streams[i]->time_base;
            //out_stream->codec->time_base.den = 25;

            out_stream->codec->sample_aspect_ratio = in_fxt->streams[i]->codec->sample_aspect_ratio;
            out_stream->codec->pix_fmt = in_fxt->streams[i]->codec->pix_fmt;

            out_stream->avg_frame_rate.den = out_stream->codec->time_base.num;
            out_stream->avg_frame_rate.num = out_stream->codec->time_base.den;
            if (!out_stream->codec->codec) {
                printf("can not find the encoder!\n");
                goto end;
            }
            if ((avcodec_open2(out_stream->codec, out_stream->codec->codec, NULL)) < 0) {
                printf("can not open the encoder\n");
                goto end;
            }
            if (out_fxt->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
            break;
        }
    }

    if (-1 == video_index) {
        printf("found no video stream in input file!\n");
        goto end;
    }

    if (!(out_fxt->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&out_fxt->pb, argv[2], AVIO_FLAG_WRITE) < 0) {
            printf("can not open output file handle!\n");
            goto end;
        }
    }

    if (avformat_write_header(out_fxt, NULL) < 0) {
        printf("can not write the header of the output file!\n");
        goto end;
    }

    AVPacket pkt_in, pkt_out;
    AVFrame *frame;
    frame = av_frame_alloc();
    av_init_packet(&pkt_in);
    av_init_packet(&pkt_out);
    int got_frame, got_picture;
    int i = 0, frame_index = 0;
    while (1) {
        got_frame = -1;
        got_picture = -1;
        if (av_read_frame(in_fxt, &pkt_in) < 0) {
            break;
        }
        if (avcodec_decode_video2(in_fxt->streams[video_index]->codec, frame, &got_frame, &pkt_in) < 0) {
            printf("can not decoder a frame");
            break;
        }
        av_free_packet(&pkt_in);

        if (got_frame) {
            frame->pts = i++;
            pkt_out.data = NULL;//主要这里必须自己初始化，或者必须置为null，不然ff_alloc_packet2函数会报错
            pkt_out.size = 0;
            if (avcodec_encode_video2(out_stream->codec, &pkt_out, frame, &got_picture) < 0) {
                printf("can not encode a frame!\n");
                break;
            }

            if (got_picture) {
                printf("Succeed to encode frame: %5d\tsize:%5d\n", frame_index, pkt_out.size);
                pkt_out.stream_index = out_stream->index;
                frame_index++;
                av_write_frame(out_fxt, &pkt_out);
                av_free_packet(&pkt_out);
            }
        }
    }
    av_frame_free(&frame);

    av_write_trailer(out_fxt);

    //clean
    avcodec_close(out_stream->codec);
    avcodec_close(out_fxt->streams[video_index]->codec);
    end:
    avformat_close_input(&in_fxt);

    if (out_fxt && !(out_fxt->oformat->flags & AVFMT_NOFILE)) {
        avio_close(out_fxt->pb);
    }
    avformat_free_context(out_fxt);
    return 0;
}

int test_avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options) {
    int ret = 0;
    AVDictionary *tmp = NULL;
    const AVPixFmtDescriptor *pixdesc;

    if (avcodec_is_open(avctx))
        return 0;

    if ((!codec && !avctx->codec)) {
        av_log(avctx, AV_LOG_ERROR, "No codec provided to avcodec_open2()\n");
        return AVERROR(EINVAL);
    }
    if ((codec && avctx->codec && codec!= avctx->codec)) {
        av_log(avctx, AV_LOG_ERROR, "This AVCodecContext was allocated for %s, "
                                    "but %s passed to avcodec_open2()\n", avctx->codec->name, codec->name);
        return AVERROR(EINVAL);
    }
    if (!codec)
        codec = avctx->codec;

    if (avctx->extradata_size < 0 || avctx->extradata_size >= FF_MAX_EXTRADATA_SIZE)
        return AVERROR(EINVAL);

    if (options)
        av_dict_copy(&tmp, *options,
                     0);

    ret = ff_lock_avcodec(avctx, codec);
    if (ret < 0)
        return
                ret;

    avctx->
            internal = static_cast<AVCodecInternal *>(av_mallocz(sizeof(AVCodecInternal)));
    if (!avctx->internal) {
        ret = AVERROR(ENOMEM);
        goto
                end;
    }

    avctx->internal->
            pool = static_cast<FramePool *>(av_mallocz(sizeof(*avctx->internal->pool)));
    if (!avctx->internal->pool) {
        ret = AVERROR(ENOMEM);
        goto
                free_and_end;
    }

    avctx->internal->to_free = av_frame_alloc();
    if (!avctx->internal->to_free) {
        ret = AVERROR(ENOMEM);
        goto
                free_and_end;
    }

    avctx->internal->compat_decode_frame = av_frame_alloc();
    if (!avctx->internal->compat_decode_frame) {
        ret = AVERROR(ENOMEM);
        goto
                free_and_end;
    }

    avctx->internal->
            buffer_frame = av_frame_alloc();
    if (!avctx->internal->buffer_frame) {
        ret = AVERROR(ENOMEM);
        goto
                free_and_end;
    }

    avctx->internal->
            buffer_pkt = av_packet_alloc();
    if (!avctx->internal->buffer_pkt) {
        ret = AVERROR(ENOMEM);
        goto
                free_and_end;
    }

    avctx->internal->ds.
            in_pkt = av_packet_alloc();
    if (!avctx->internal->ds.in_pkt) {
        ret = AVERROR(ENOMEM);
        goto
                free_and_end;
    }

    avctx->internal->
            last_pkt_props = av_packet_alloc();
    if (!avctx->internal->last_pkt_props) {
        ret = AVERROR(ENOMEM);
        goto
                free_and_end;
    }

    avctx->internal->
            skip_samples_multiplier = 1;

    if (codec->priv_data_size > 0) {
        if (!avctx->priv_data) {
            avctx->
                    priv_data = av_mallocz(codec->priv_data_size);
            if (!avctx->priv_data) {
                ret = AVERROR(ENOMEM);
                goto
                        end;
            }
            if (codec->priv_class) {
                *(const AVClass **) avctx->
                        priv_data = codec->priv_class;
                av_opt_set_defaults(avctx
                                            ->priv_data);
            }
        }
        if (codec->
                priv_class && (ret = av_opt_set_dict(avctx->priv_data, &tmp)
                              ) < 0)
            goto
                    free_and_end;
    } else {
        avctx->
                priv_data = NULL;
    }
    if ((
                ret = av_opt_set_dict(avctx, &tmp)
        ) < 0)
        goto
                free_and_end;

    if (avctx->
            codec_whitelist && av_match_list(codec->name, avctx->codec_whitelist, ',')
                               <= 0) {
        av_log(avctx, AV_LOG_ERROR, "Codec (%s) not on whitelist \'%s\'\n", codec->name, avctx->codec_whitelist);
        ret = AVERROR(EINVAL);
        goto
                free_and_end;
    }

// only call ff_set_dimensions() for non H.264/VP6F/DXV codecs so as not to overwrite previously setup dimensions
    if (!(avctx->
            coded_width && avctx
                  ->
                          coded_height && avctx
                  ->
                          width && avctx
                  ->
                          height &&
          (avctx
                   ->codec_id == AV_CODEC_ID_H264 || avctx->codec_id == AV_CODEC_ID_VP6F ||
           avctx->codec_id == AV_CODEC_ID_DXV))) {
        if (avctx->
                coded_width && avctx
                    ->coded_height)
            ret = ff_set_dimensions(avctx, avctx->coded_width, avctx->coded_height);
        else if (avctx->
                width && avctx
                         ->height)
            ret = ff_set_dimensions(avctx, avctx->width, avctx->height);
        if (ret < 0)
            goto
                    free_and_end;
    }

    if ((avctx->coded_width || avctx->coded_height || avctx->width || avctx->height)
        && (
                av_image_check_size2(avctx
                                             ->coded_width, avctx->coded_height, avctx->max_pixels, AV_PIX_FMT_NONE, 0,
                                     avctx) < 0
                ||
                av_image_check_size2(avctx
                                             ->width, avctx->height, avctx->max_pixels, AV_PIX_FMT_NONE, 0, avctx) <
                0)) {
        av_log(avctx, AV_LOG_WARNING, "Ignoring invalid width/height values\n");
        ff_set_dimensions(avctx,
                          0, 0);
    }

    if (avctx->width > 0 && avctx->height > 0) {
        if (
                av_image_check_sar(avctx
                                           ->width, avctx->height,
                                   avctx->sample_aspect_ratio) < 0) {
            av_log(avctx, AV_LOG_WARNING, "ignoring invalid SAR: %u/%u\n",
                   avctx->sample_aspect_ratio.num,
                   avctx->sample_aspect_ratio.den);
            avctx->
                    sample_aspect_ratio = (AVRational) {0, 1};
        }
    }

/* if the decoder init function was already called previously,
 * free the already allocated subtitle_header before overwriting it */
    if (
            av_codec_is_decoder(codec)
            )
        av_freep(&avctx
                ->subtitle_header);

    if (avctx->channels > FF_SANE_NB_CHANNELS) {
        ret = AVERROR(EINVAL);
        goto
                free_and_end;
    }

    avctx->
            codec = codec;
    if ((avctx->codec_type == AVMEDIA_TYPE_UNKNOWN || avctx->codec_type == codec->type) &&
        avctx->codec_id == AV_CODEC_ID_NONE) {
        avctx->
                codec_type = codec->type;
        avctx->
                codec_id = codec->id;
    }
    if (avctx->codec_id != codec->id || (avctx->codec_type != codec->
            type
                                         && avctx
                                                    ->codec_type != AVMEDIA_TYPE_ATTACHMENT)) {
        av_log(avctx, AV_LOG_ERROR, "Codec type or id mismatches\n");
        ret = AVERROR(EINVAL);
        goto
                free_and_end;
    }
    avctx->
            frame_number = 0;
    avctx->
            codec_descriptor = avcodec_descriptor_get(avctx->codec_id);

    if ((avctx->codec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) &&
        avctx->strict_std_compliance > FF_COMPLIANCE_EXPERIMENTAL) {
        const char *codec_string = av_codec_is_encoder(codec) ? "encoder" : "decoder";
        AVCodec *codec2;
        av_log(avctx, AV_LOG_ERROR,
               "The %s '%s' is experimental but experimental codecs are not enabled, "
               "add '-strict %d' if you want to use it.\n",
               codec_string, codec->name, FF_COMPLIANCE_EXPERIMENTAL);
        codec2 = av_codec_is_encoder(codec) ? avcodec_find_encoder(codec->id) : avcodec_find_decoder(codec->id);
        if (!(codec2->capabilities & AV_CODEC_CAP_EXPERIMENTAL))
            av_log(avctx, AV_LOG_ERROR, "Alternatively use the non experimental %s '%s'.\n",
                   codec_string, codec2->name);
        ret = AVERROR_EXPERIMENTAL;
        goto
                free_and_end;
    }

    if (avctx->codec_type == AVMEDIA_TYPE_AUDIO &&
        (!avctx->time_base.num || !avctx->time_base.den)) {
        avctx->time_base.
                num = 1;
        avctx->time_base.
                den = avctx->sample_rate;
    }

    if (!HAVE_THREADS)
        av_log(avctx, AV_LOG_WARNING, "Warning: not compiled with thread support, using thread emulation\n");

//    if (CONFIG_FRAME_THREAD_ENCODER && av_codec_is_encoder(avctx->codec)) {
//        ff_unlock_avcodec(
//                codec); //we will instantiate a few encoders thus kick the counter to prevent false detection of a problem
//        ret = ff_frame_thread_encoder_init(avctx, options ? *options : NULL);
//        ff_lock_avcodec(avctx, codec
//        );
//        if (ret < 0)
//            goto
//                    free_and_end;
//    }
//
//    if (HAVE_THREADS
//        && !(avctx->internal->
//            frame_thread_encoder && (avctx
//                                             ->active_thread_type & FF_THREAD_FRAME))) {
//        ret = ff_thread_init(avctx);
//        if (ret < 0) {
//            goto
//                    free_and_end;
//        }
//    }
    if (!HAVE_THREADS && !(codec->capabilities & AV_CODEC_CAP_AUTO_THREADS))
        avctx->
                thread_count = 1;

    if (avctx->codec->max_lowres < avctx->lowres || avctx->lowres < 0) {
        av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n",
               avctx->codec->max_lowres);
        avctx->
                lowres = avctx->codec->max_lowres;
    }

#if FF_API_VISMV
    if (avctx->debug_mv)
        av_log(avctx, AV_LOG_WARNING, "The 'vismv' option is deprecated, "
                                      "see the codecview filter instead.\n");
#endif

    if (
            av_codec_is_encoder(avctx
                                        ->codec)) {
        int i;
#if FF_API_CODED_FRAME
//        FF_DISABLE_DEPRECATION_WARNINGS
        avctx->coded_frame = av_frame_alloc();
        if (!avctx->coded_frame) {
            ret = AVERROR(ENOMEM);
            goto
                    free_and_end;
        }
//        FF_ENABLE_DEPRECATION_WARNINGS
#endif

        if (avctx->time_base.num <= 0 || avctx->time_base.den <= 0) {
            av_log(avctx, AV_LOG_ERROR, "The encoder timebase is not set.\n");
            ret = AVERROR(EINVAL);
            goto
                    free_and_end;
        }

        if (avctx->codec->sample_fmts) {
            for (
                    i = 0;
                    avctx->codec->sample_fmts[i] !=
                    AV_SAMPLE_FMT_NONE;
                    i++) {
                if (avctx->sample_fmt == avctx->codec->sample_fmts[i])
                    break;
                if (avctx->channels == 1 &&
                    av_get_planar_sample_fmt(avctx
                                                     ->sample_fmt) ==
                    av_get_planar_sample_fmt(avctx
                                                     ->codec->sample_fmts[i])) {
                    avctx->
                            sample_fmt = avctx->codec->sample_fmts[i];
                    break;
                }
            }
            if (avctx->codec->sample_fmts[i] == AV_SAMPLE_FMT_NONE) {
                char buf[128];
                snprintf(buf,
                         sizeof(buf), "%d", avctx->sample_fmt);
                av_log(avctx, AV_LOG_ERROR, "Specified sample format %s is invalid or not supported\n",
                       (char *)
                               av_x_if_null(av_get_sample_fmt_name(avctx->sample_fmt), buf
                               ));
                ret = AVERROR(EINVAL);
                goto
                        free_and_end;
            }
        }
        if (avctx->codec->pix_fmts) {
            for (
                    i = 0;
                    avctx->codec->pix_fmts[i] !=
                    AV_PIX_FMT_NONE;
                    i++)
                if (avctx->pix_fmt == avctx->codec->pix_fmts[i])
                    break;
            if (avctx->codec->pix_fmts[i] == AV_PIX_FMT_NONE
                && !((avctx->codec_id == AV_CODEC_ID_MJPEG || avctx->codec_id == AV_CODEC_ID_LJPEG)
                     && avctx->strict_std_compliance <= FF_COMPLIANCE_UNOFFICIAL)) {
                char buf[128];
                snprintf(buf,
                         sizeof(buf), "%d", avctx->pix_fmt);
                av_log(avctx, AV_LOG_ERROR, "Specified pixel format %s is invalid or not supported\n",
                       (char *)
                               av_x_if_null(av_get_pix_fmt_name(avctx->pix_fmt), buf
                               ));
                ret = AVERROR(EINVAL);
                goto
                        free_and_end;
            }
            if (avctx->codec->pix_fmts[i] == AV_PIX_FMT_YUVJ420P ||
                avctx->codec->pix_fmts[i] == AV_PIX_FMT_YUVJ411P ||
                avctx->codec->pix_fmts[i] == AV_PIX_FMT_YUVJ422P ||
                avctx->codec->pix_fmts[i] == AV_PIX_FMT_YUVJ440P ||
                avctx->codec->pix_fmts[i] == AV_PIX_FMT_YUVJ444P)
                avctx->
                        color_range = AVCOL_RANGE_JPEG;
        }
        if (avctx->codec->supported_samplerates) {
            for (
                    i = 0;
                    avctx->codec->supported_samplerates[i] != 0; i++)
                if (avctx->sample_rate == avctx->codec->supported_samplerates[i])
                    break;
            if (avctx->codec->supported_samplerates[i] == 0) {
                av_log(avctx, AV_LOG_ERROR, "Specified sample rate %d is not supported\n",
                       avctx->sample_rate);
                ret = AVERROR(EINVAL);
                goto
                        free_and_end;
            }
        }
        if (avctx->sample_rate < 0) {
            av_log(avctx, AV_LOG_ERROR, "Specified sample rate %d is not supported\n",
                   avctx->sample_rate);
            ret = AVERROR(EINVAL);
            goto
                    free_and_end;
        }
        if (avctx->codec->channel_layouts) {
            if (!avctx->channel_layout) {
                av_log(avctx, AV_LOG_WARNING, "Channel layout not specified\n");
            } else {
                for (
                        i = 0;
                        avctx->codec->channel_layouts[i] != 0; i++)
                    if (avctx->channel_layout == avctx->codec->channel_layouts[i])
                        break;
                if (avctx->codec->channel_layouts[i] == 0) {
                    char buf[512];
                    av_get_channel_layout_string(buf,
                                                 sizeof(buf), -1, avctx->channel_layout);
                    av_log(avctx, AV_LOG_ERROR, "Specified channel layout '%s' is not supported\n", buf);
                    ret = AVERROR(EINVAL);
                    goto
                            free_and_end;
                }
            }
        }
        if (avctx->
                channel_layout && avctx
                    ->channels) {
            int channels = av_get_channel_layout_nb_channels(avctx->channel_layout);
            if (channels != avctx->channels) {
                char buf[512];
                av_get_channel_layout_string(buf,
                                             sizeof(buf), -1, avctx->channel_layout);
                av_log(avctx, AV_LOG_ERROR,
                       "Channel layout '%s' with %d channels does not match number of specified channels %d\n",
                       buf, channels, avctx->channels);
                ret = AVERROR(EINVAL);
                goto
                        free_and_end;
            }
        } else if (avctx->channel_layout) {
            avctx->
                    channels = av_get_channel_layout_nb_channels(avctx->channel_layout);
        }
        if (avctx->channels < 0) {
            av_log(avctx, AV_LOG_ERROR, "Specified number of channels %d is not supported\n",
                   avctx->channels);
            ret = AVERROR(EINVAL);
            goto
                    free_and_end;
        }
        if (avctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            pixdesc = av_pix_fmt_desc_get(avctx->pix_fmt);
            if (avctx->bits_per_raw_sample < 0
                || (avctx->bits_per_raw_sample > 8 && pixdesc->comp[0].depth <= 8)) {
                av_log(avctx, AV_LOG_WARNING,
                       "Specified bit depth %d not possible with the specified pixel formats depth %d\n",
                       avctx->bits_per_raw_sample, pixdesc->comp[0].depth);
                avctx->
                        bits_per_raw_sample = pixdesc->comp[0].depth;
            }
            if (avctx->width <= 0 || avctx->height <= 0) {
                av_log(avctx, AV_LOG_ERROR, "dimensions not set\n");
                ret = AVERROR(EINVAL);
                goto
                        free_and_end;
            }
        }
        if ((avctx->codec_type == AVMEDIA_TYPE_VIDEO || avctx->codec_type == AVMEDIA_TYPE_AUDIO)
            && avctx->bit_rate > 0 && avctx->bit_rate < 1000) {
//            av_log(avctx, AV_LOG_WARNING, "Bitrate %" PRId64 " is extremely low, maybe you mean %" PRId64 "k\n",
//                   avctx->bit_rate, avctx->bit_rate);
        }

        if (!avctx->rc_initial_buffer_occupancy)
            avctx->
                    rc_initial_buffer_occupancy = avctx->rc_buffer_size * 3LL / 4;

        if (avctx->
                ticks_per_frame && avctx
                    ->time_base.
                num &&
            avctx
                    ->ticks_per_frame > INT_MAX / avctx->time_base.num) {
            av_log(avctx, AV_LOG_ERROR,
                   "ticks_per_frame %d too large for the timebase %d/%d.",
                   avctx->ticks_per_frame,
                   avctx->time_base.num,
                   avctx->time_base.den);
            goto
                    free_and_end;
        }

//        if (avctx->hw_frames_ctx) {
//            AVHWFramesContext *frames_ctx = (AVHWFramesContext *) avctx->hw_frames_ctx->data;
//            if (frames_ctx->format != avctx->pix_fmt) {
//                av_log(avctx, AV_LOG_ERROR,
//                       "Mismatching AVCodecContext.pix_fmt and AVHWFramesContext.format\n");
//                ret = AVERROR(EINVAL);
//                goto
//                        free_and_end;
//            }
//            if (avctx->sw_pix_fmt !=
//                AV_PIX_FMT_NONE &&
//                avctx
//                        ->sw_pix_fmt != frames_ctx->sw_format) {
//                av_log(avctx, AV_LOG_ERROR,
//                       "Mismatching AVCodecContext.sw_pix_fmt (%s) "
//                       "and AVHWFramesContext.sw_format (%s)\n",
//                       av_get_pix_fmt_name(avctx
//                                                   ->sw_pix_fmt),
//                       av_get_pix_fmt_name(frames_ctx
//                                                   ->sw_format));
//                ret = AVERROR(EINVAL);
//                goto
//                        free_and_end;
//            }
//            avctx->
//                    sw_pix_fmt = frames_ctx->sw_format;
//        }
    }

    avctx->
            pts_correction_num_faulty_pts =
    avctx->pts_correction_num_faulty_dts = 0;
    avctx->
            pts_correction_last_pts =
    avctx->pts_correction_last_dts = INT64_MIN;

    if (!CONFIG_GRAY && avctx
                                       ->flags & AV_CODEC_FLAG_GRAY
        && avctx->codec_descriptor->type == AVMEDIA_TYPE_VIDEO)
        av_log(avctx, AV_LOG_WARNING,
               "gray decoding requested but not enabled at configuration time\n");

    if (avctx->codec->init && (!(avctx->active_thread_type & FF_THREAD_FRAME)
                               || avctx->internal->frame_thread_encoder)) {
        ret = avctx->codec->init(avctx);
        if (ret < 0) {
            goto
                    free_and_end;
        }
    }

    ret = 0;

#if FF_API_AUDIOENC_DELAY
    if (
            av_codec_is_encoder(avctx
                                        ->codec))
        avctx->
                delay = avctx->initial_padding;
#endif

    if (
            av_codec_is_decoder(avctx
                                        ->codec)) {
//        if (!avctx->bit_rate)
//            avctx->bit_rate = get_bit_rate(avctx);
/* validate channel layout from the decoder */
        if (avctx->channel_layout) {
            int channels = av_get_channel_layout_nb_channels(avctx->channel_layout);
            if (!avctx->channels)
                avctx->
                        channels = channels;
            else if (channels != avctx->channels) {
                char buf[512];
                av_get_channel_layout_string(buf,
                                             sizeof(buf), -1, avctx->channel_layout);
                av_log(avctx, AV_LOG_WARNING,
                       "Channel layout '%s' with %d channels does not match specified number of channels %d: "
                       "ignoring specified channel layout\n",
                       buf, channels, avctx->channels);
                avctx->
                        channel_layout = 0;
            }
        }
        if (avctx->
                channels && avctx
                                    ->
                                            channels < 0 ||
            avctx->channels > FF_SANE_NB_CHANNELS
                ) {
            ret = AVERROR(EINVAL);
            goto
                    free_and_end;
        }
        if (avctx->sub_charenc) {
            if (avctx->codec_type != AVMEDIA_TYPE_SUBTITLE) {
                av_log(avctx, AV_LOG_ERROR, "Character encoding is only "
                                            "supported with subtitles codecs\n");
                ret = AVERROR(EINVAL);
                goto
                        free_and_end;
            } else if (avctx->codec_descriptor->props & AV_CODEC_PROP_BITMAP_SUB) {
                av_log(avctx, AV_LOG_WARNING, "Codec '%s' is bitmap-based, "
                                              "subtitles character encoding will be ignored\n",
                       avctx->codec_descriptor->name);
                avctx->
                        sub_charenc_mode = FF_SUB_CHARENC_MODE_DO_NOTHING;
            } else {
/* input character encoding is set for a text based subtitle
 * codec at this point */
                if (avctx->sub_charenc_mode == FF_SUB_CHARENC_MODE_AUTOMATIC)
                    avctx->
                            sub_charenc_mode = FF_SUB_CHARENC_MODE_PRE_DECODER;

                if (avctx->sub_charenc_mode == FF_SUB_CHARENC_MODE_PRE_DECODER) {
#if CONFIG_ICONV
                    iconv_t cd = iconv_open("UTF-8", avctx->sub_charenc);
                                        if (cd == (iconv_t)-1) {
                                            ret = AVERROR(errno);
                                            av_log(avctx, AV_LOG_ERROR, "Unable to open iconv context "
                                                   "with input character encoding \"%s\"\n", avctx->sub_charenc);
                                            goto free_and_end;
                                        }
                                        iconv_close(cd);
#else
                    av_log(avctx, AV_LOG_ERROR, "Character encoding subtitles "
                                                "conversion needs a libavcodec built with iconv support "
                                                "for this codec\n");
                    ret = AVERROR(ENOSYS);
                    goto
                            free_and_end;
#endif
                }
            }
        }

#if FF_API_AVCTX_TIMEBASE
        if (avctx->framerate.num > 0 && avctx->framerate.den > 0)
            avctx->
                    time_base = av_inv_q(av_mul_q(avctx->framerate, (AVRational) {avctx->ticks_per_frame, 1}));
#endif
    }
    if (codec->priv_data_size > 0 && avctx->
            priv_data && codec
                ->priv_class) {
        av_assert0(*(const AVClass **) avctx->priv_data == codec->priv_class);
    }

    end:
    ff_unlock_avcodec(codec);
    if (options) {
        av_dict_free(options);
        *
                options = tmp;
    }

    return
            ret;
    free_and_end:
    if (avctx->
            codec &&
        (avctx
                 ->codec->
                caps_internal & FF_CODEC_CAP_INIT_CLEANUP
        ))
        avctx->codec->
                close(avctx);

    if (codec->
            priv_class && codec
                ->priv_data_size)
        av_opt_free(avctx
                            ->priv_data);
    av_opt_free(avctx);

#if FF_API_CODED_FRAME
//    FF_DISABLE_DEPRECATION_WARNINGS
            av_frame_free(&avctx->coded_frame);
//    FF_ENABLE_DEPRECATION_WARNINGS
#endif

            av_dict_free(&tmp);
    av_freep(&avctx
            ->priv_data);
    if (avctx->internal) {
        av_frame_free(&avctx
                ->internal->to_free);
        av_frame_free(&avctx
                ->internal->compat_decode_frame);
        av_frame_free(&avctx
                ->internal->buffer_frame);
        av_packet_free(&avctx
                ->internal->buffer_pkt);
        av_packet_free(&avctx
                ->internal->last_pkt_props);

        av_packet_free(&avctx
                ->internal->ds.in_pkt);

        av_freep(&avctx
                ->internal->pool);
    }
    av_freep(&avctx
            ->internal);
    avctx->
            codec = NULL;
    goto
            end;
}