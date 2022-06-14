//
// Created by mochangsheng on 2019/6/18.
//

#ifndef MULTIMEDIAEXAMPLE_COMPAT_H
#define MULTIMEDIAEXAMPLE_COMPAT_H

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <libavutil/error.h>
#include <libavutil/timestamp.h>

#if defined(__cplusplus)
}
#endif

//兼容av_err2str，避免在C++中编译异常
static char av_error[AV_ERROR_MAX_STRING_SIZE] = { 0 };
#define av_err2str_ffmpeg(errnum) \
    av_make_error_string(av_error, AV_ERROR_MAX_STRING_SIZE, errnum)

/**
* Convenience macro, the return value should be used only directly in
* function arguments but never stand-alone.
*/
static char av_ts_error[AV_TS_MAX_STRING_SIZE] = { 0 };
#define av_ts2str_ffmpeg(ts) av_ts_make_string(av_ts_error, ts)

/**
 * Convenience macro, the return value should be used only directly in
 * function arguments but never stand-alone.
 */
static char av_ts2timestr_error[AV_TS_MAX_STRING_SIZE] = { 0 };
#define av_ts2timestr_ffmpeg(ts, tb) av_ts_make_time_string(av_ts2timestr_error, ts, tb)

#endif //MULTIMEDIAEXAMPLE_COMPAT_H
