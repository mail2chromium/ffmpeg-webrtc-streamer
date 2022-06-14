cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

# 定义C++的编译配置
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11 -D__STDC_CONSTANT_MACROS -fpermissive")

set(LIB_AVCODEC avcodec)
set(LIB_AVDEVICE avdevice)
set(LIB_AVFILTER avfilter)
set(LIB_AVFORMAT avformat)
set(LIB_AVUTIL avutil)
set(LIB_POSTPROC postproc)
set(LIB_SWRESAMPLE swresample)
set(LIB_SWSCALE swscale)

set(EXTERNAL_LIB -lx264
        -lfdk-aac
        -lm
        -lz
        -ldl
        -lpthread
        -lxcb
        -lXau
        -lxcb-shape
        -lxcb-xfixes
        -lxcb-render
        -llzma
        -lxcb-shm)

include_directories(include/compat)


add_executable(EncodeVideo EncodeVideo.cpp)
target_link_libraries(EncodeVideo  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(OutputFFmpgeInfo OutputFFmpgeInfo.cpp)
target_link_libraries(OutputFFmpgeInfo  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(EncodeYuv420pToH264 EncodeYuv420pToH264.cpp)
target_link_libraries(EncodeYuv420pToH264  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(EncodeAudio EncodeAudio.cpp)
target_link_libraries(EncodeAudio  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(Muxing Muxing.cpp)
target_link_libraries(Muxing  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(simplest_ffmpeg_muxer simplest_ffmpeg_muxer.cpp)
target_link_libraries(simplest_ffmpeg_muxer  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(AvIOContextTest AvIOContextTest.cpp)
target_link_libraries(AvIOContextTest  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(ParseDecode ParseDecode.cpp)
target_link_libraries(ParseDecode  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(TranscodeAac TranscodeAac.cpp)
target_link_libraries(TranscodeAac  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(AacCodec AacCodec.cpp)
target_link_libraries(AacCodec  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(PcmToAac PcmToAac.cpp)
target_link_libraries(PcmToAac  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(audio_fifo audio_fifo.cpp)
target_link_libraries(audio_fifo  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(transcoding transcoding.cpp)
target_link_libraries(transcoding  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(ScreenRecorder ScreenRecorder.cpp)
target_link_libraries(ScreenRecorder  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(RTMPMuxing RTMPMuxing.cpp)
target_link_libraries(RTMPMuxing
        ${LIB_POSTPROC}
        ${LIB_AVFORMAT}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVCODEC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${LIB_AVUTIL}
        ${EXTERNAL_LIB})

add_executable(FilteringVideo FilteringVideo.cpp)
target_link_libraries(FilteringVideo  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})

add_executable(CustomFFEncoder CustomFFEncoder.cpp)
target_link_libraries(CustomFFEncoder  ${LIB_AVCODEC}
        ${LIB_AVDEVICE}
        ${LIB_AVFILTER}
        ${LIB_AVFORMAT}
        ${LIB_AVUTIL}
        ${LIB_POSTPROC}
        ${LIB_SWRESAMPLE}
        ${LIB_SWSCALE}
        ${EXTERNAL_LIB})