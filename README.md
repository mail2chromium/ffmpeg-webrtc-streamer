# ffmpeg-webrtc-streamer
Stream FFMPEG based Audio and Video using WebRtc. It is the most fastest P2P based streamer which gets Audio and Video from FFMPEG and then stream it to **WebRtc Endpoints** (Android, iOS, Web) and **Media Servers** like *Ant-Media*.


## Quick Usage:
-----

If you are running this first time. Just run the following commands to get executables:

```
  $ git clone https://github.com/mail2chromium/ffmpeg-webrtc-streamer.git
  
  $ cd ffmpeg-webrtc-streamer/

  $ chmod +x Setup.sh

  $ ./Setup.sh

  $ chmod +x run-install.sh

  $ ./run-install.sh

```
At this moment, you can only run `Audio Streaming or Video Streaming`. As both are separately handled. But in coming research, Audio and video from `FFMPEG` will be streamed using webrtc at the same time.

Run the streamer to test Audio Streaming:
-----

```
# Path to executable
  $ cd build/

# To get raw audio packets and stream using webrtc

  $ ./ffmpeg -y -re -f pulse -i default -ac 1 -sample_rate 48000 -b:a 32000 output_audio.wav


# To get pre-encoded audio packets and stream using webrtc


  $ ./ffmpeg -re -y -f pulse -channel_layout stereo -i default -c:a opus -strict -2 -sample_rate 48k -b:a 32k out.ogg

```

Once your streamer is running, then **Open** `index.html` in chrome browser. Add the following values and simple `connect`;

```
  Host: localhost

  Port: 4321

```

Run the streamer to test Video Streaming:
----
```
# Path to executable
  $ cd build/

# To get raw video frames and stream using webrtc

  $ ./ffmpeg -re -y -f v4l2 -framerate 25 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 10 -b:v 512k -s 640x480 -c:v rawvideo -ac 2 -ab 32k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 output.yuv


# To get h264 encoded video frames and stream using webrtc


  $ ./ffmpeg -re -y -f v4l2 -framerate 60 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 10 -b:v 1024k -ac 2 -ab 32k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 -vcodec h264 output.yuv

```

Once your streamer is running, then **Open** `index.html` in chrome browser. Add the following values and simple `connect`;

```
  Host: localhost

  Port: 4321

```


## Explanation of Setup:
-----

Simply clone the project using the following command;

```

  $ git clone https://github.com/mail2chromium/ffmpeg-webrtc-streamer.git
  
  $ cd ffmpeg-webrtc-streamer/

```

I have added all the necessary packages into a `Setup.sh` file which is present in project's main directory. Simply run this file:

> If you have already cloned this repository, then there is no need to run this script again. 

```

  $ ./Setup.sh

```

The above process might take some time based on your system's specs and internet speed. Please wait untill all the necessary packages are installed properly.

## Build & Compilation:
------

Make sure you are in project main directory `ffmpeg-webrtc-streamer`. For Build and compilation of this project run the following commands;

```

  $ chmod +x run-install.sh && ./run-install.sh

```

I have made a simple script that will install the necessary things inside the `build` directory;

```

DEST="./build/"
echo "$DEST"
git fetch --all
git pull origin main

if [ -d "$DEST" ]; then
#  if [ -L "$DEST" ]; then
    # It is a symbolic links #
    echo "Directory found. Lets make it and proceed..."
    cd build
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
    make

else
    # It is a directory #
    echo "Directory not found. Lets proceed..."
    mkdir build && cd build
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
    make
#  fi
fi

```

> The above process might take some time based on your system's specs and internet speed. Please wait untill all the necessary pa ckages are installed properly. 

This script will fetch the latest changes from the git repo and make necessary changes to the project by running `make` command.

Once the build and compilation is done, you can find the executable of ffmpeg inside `build` directory; Now using the following commands, you can either pass raw video frames or encoded video frames from FFMPEG to webrtc native stack for streaming;

```
# To get raw video frames and stream using webrtc

  $ ./ffmpeg -re -y -f v4l2 -framerate 25 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 10 -b:v 512k -s 640x480 -c:v rawvideo -ac 2 -ab 32k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 output.yuv


# To get h264 encoded video frames and stream using webrtc


  $ ./ffmpeg -re -y -f v4l2 -framerate 60 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 10 -b:v 1024k -ac 2 -ab 32k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 -vcodec h264 output.yuv

```

Once the executable is running smoothly, then you can move to `client` directory and simply open `index.html` in chrome browser to observe real-time streaming from ffmpeg using webrtc.
