# ffmpeg-webrtc-streamer
Stream FFMPEG based Audio and Video using WebRtc. It is the most fastest P2P based streamer which gets Audio and Video from FFMPEG and then stream it to **WebRtc Endpoints** (Android, iOS, Web) and **Media Servers** like *Ant-Media*.

#### Important Note:
   **To get better results, Please test this on local deployed AntMedia Server.**

## Quick Usage:
-----

If you are running this first time. Just run the following commands to get executables. 

## For AntMedia Publisher:

```
  $ git clone https://github.com/mail2chromium/ffmpeg-webrtc-streamer.git -b antmedia_publisher
  
  $ cd ffmpeg-webrtc-streamer/

  $ chmod +x Setup.sh

  $ ./Setup.sh

  $ chmod +x run-install.sh

  $ ./run-install.sh

```

### Run the Publisher:
-----

```
  # Path to executable
  $ cd build/
  
  # Raw Video
  $ ./ffmpeg -re -y -f v4l2 -framerate 60 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -tune zerolatency -r 91 -b:v 512k -c:v rawvideo -s 640x480 -ac 2 -input_format yuv420p -update 1 -preset ultrafast -f mpegts ws://localhost:5080/WebRTCAppEE/websocket/id=rtp_stream

  # H264 Video
  $ ./ffmpeg -re -y -f v4l2 -framerate 60 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 91 -b:v 512k -ac 2 -ab 96k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 -vcodec h264 -f mpegts ws://localhost:5080/WebRTCAppEE/websocket/id=rtp_stream

  #Raw Video & Audio
  $ ./ffmpeg -re -y -f v4l2 -framerate 60 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -tune zerolatency -r 10 -b:v 512k -c:v rawvideo -s 640x480 -ac 2 -b:a 32000 -input_format yuv420p -update 1 -preset ultrafast -f mpegts ws://localhost:5080/WebRTCAppEE/websocket/id=rtp_stream

  # H264 Video & Audio
  ./ffmpeg -re -y -f v4l2 -framerate 60 -video_size 640x480 -i /dev/video0 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -r 91 -b:v 512k -b:a 32000 -ac 2 -ab 96k -ar 44100 -vf format=yuv420p -update 1 -flush_packets 0 -vcodec h264 out.yuv -f mpegts ws://localhost:5080/WebRTCAppEE/websocket/id=rtp_stream

```

## For AntMedia Subscriber:



```
  $ git clone https://github.com/mail2chromium/ffmpeg-webrtc-streamer.git -b antmedia_subscriber
  
  $ cd ffmpeg-webrtc-streamer/

  $ chmod +x Setup.sh

  $ ./Setup.sh

  $ chmod +x run-install.sh

  $ ./run-install.sh

```

### Run Subscriber:

```
  $ cd build/

  # ffplay
  $ ./ffplay ws://localhost:5080/WebRTCAppEE/websocket/id=rtp_stream

```


