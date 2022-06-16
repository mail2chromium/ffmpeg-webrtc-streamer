# ffmpeg-webrtc-streamer
Stream FFMPEG based Audio and Video using WebRtc. It is the most fastest P2P based streamer which gets Audio and Video from FFMPEG and then stream it to **WebRtc Endpoints** (Android, iOS, Web) and **Media Servers** (Ant-Media, Janus, Jitsi, Kurento)

## Basic Setup:
-----

Simply clone the project using the following command;

```

  git clone https://github.com/mail2chromium/ffmpeg-webrtc-streamer.git
  
  cd ffmpeg-webrtc-streamer/

```

I have added all the necessary packages into a `Setup.sh` file which is present in project's main directory. Simply run this file:

```

  ./Setup.sh

```

The above process might take some time based on your system's specs and internet speed. Please wait untill all the necessary packages are installed properly.

## Build & Compilation:
------

Make sure you are in project main directory. For Build and compilation of this project run the following commands;

```

  mkdir build && cd build
  
  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
  
  make

```

The above process might take some time based on your system's specs and internet speed. Please wait untill all the necessary packages are installed properly. Once the build and compilation is done, you can find the executable of ffmpeg inside `build` directory; Now using the following commands, you can either pass raw video frames or encoded video frames from FFMPEG to webrtc native stack for streaming;

```
# To get raw video frames and stream using webrtc

  ./ffmpeg -re -y -f v4l2 -framerate 25 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 10 -b:v 512k -s 640x480 -c:v rawvideo -ac 2 -ab 32k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 output.yuv




# To get h264 encoded video frames and stream using webrtc


  ./ffmpeg -re -y -f v4l2 -framerate 60 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 10 -b:v 1024k -ac 2 -ab 32k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 -vcodec h264 output.yuv

```

Once the executable is running smoothly, then you can move to `client` directory and simply open `index.html` in chrome browser to observe real-time streaming from ffmpeg using webrtc.
