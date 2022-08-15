# ffmpeg-webrtc-streamer
Stream FFMPEG based Audio and Video using WebRtc. It is the most fastest P2P based streamer which gets Audio and Video from FFMPEG and then stream it to **WebRtc Endpoints** (Android, iOS, Web) and **Media Servers** like *Ant-Media*.

#### Important Note:
   **To get better results, Please test this on local deployed AntMedia Server.**

## Quick Usage:
-----

If you are running this first time. Just run the following commands to get executables. 


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


