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

The above process might take some time based on your system's specs and internet speed. Please wait untill all the necessary packages are installed properly. Once the build and compilation is done, you can find the executable of ffmpeg inside `build` directory;

```

  ./ffmpeg

```

Once the executable is running smoothly, then you can move to `client` directory and simply open `index.html` in chrome browser to observe real-time streaming from ffmpeg using webrtc.
