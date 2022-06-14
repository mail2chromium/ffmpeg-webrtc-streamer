## 基于docker加clion配置远程调试ffmpeg，便于多种环境开发

1、构建docker容器

    # 切换到工程目录
    cd 工程目录

    # 开始构建，并后台运行容器
    docker-compose up -d

    # 如果要停止容器，则可以
    docker-compose down

2、配置clion环境

    *设置setting -> Build,Execution,Deployment->Toolchains

        配置ssh，账号：debugger  密码：pwd  端口：7776

        其他编译器选项则选用自动检测

    *设置setting -> Build,Execution,Deployment->CMake

        创建一个CMake类型，名称为RemoteHost，类型为Debug，toolchain选择为RemoteHost

    *设置setting -> Build,Execution,Deployment->Deployment

        使用默认即可

3、编译，点击ffmpeg|RemoteHostDebug即可


## 依赖
* libx264
* fdk_aac

## configure参数

```bash
configure --extra-libs=-lpthread --extra-libs=-lm --enable-gpl --enable-libfdk_aac --enable-libx264 --enable-nonfree --disable-asm
```

## 以下文件由configure和make产生

* config.h
* libavcode/bsf_list.c
* libavcode/codec_list.c
* libavcode/parser_list.c
* libavdevice/indev_list.c
* libavdevice/outdev_list.c
* libavfilter/filter_list.c
* libavformat/demuxer_list.c
* libavformat/muxer_list.c
* libavformat/protocol_list.c
* libavutil/config.h