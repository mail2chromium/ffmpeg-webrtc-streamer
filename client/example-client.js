// The WebSocket object used to manage a connection.
let webSocketConnection = null;
// The RTCPeerConnection through which we engage in the SDP handshake.
let rtcPeerConnection = null;
// The data channel used to communicate.
let dataChannel = null;

var sendButton = document.getElementById("sendButton");

// var localvid=document.getElementById('localvideo');
var remotevideo = document.getElementById("remotevideo");
var localStream = null;
// sendButton.addEventListener('click', sendMessageThroughDataChannel, false);

var messageInputBox = document.getElementById("message");

// Callback for when we receive a message on the data channel.
function onDataChannelMessage(event) {
  console.log("DataChannelMessage");
  console.log(event.data);
  document.getElementById("receiveMsg").innerHTML = event.data;
}

// Called when we are not the caller (ie we are the receiver)
// and the data channel has been opened
function handleDataChannelCreated(event) {
  console.log("dataChannel opened");

  dataChannel = event.channel;
  dataChannel.onmessage = handleDataChannelReceiveMessage;
  dataChannel.onopen = handleDataChannelStatusChange;
  dataChannel.onclose = handleDataChannelStatusChange;
}

function onDataChannelOpen(event) {
  console.log("DataChannelOpen");
  //dataChannel.send("PING");
}

function onDataChannelError(error) {
  console.log("DataChannel Error : " + error);
}

function sendMessageThroughDataChannel() {
  var message = messageInputBox.value;
  console.log("sending: " + message);
  dataChannel.send(message);

  // Clear the input box and re-focus it, so that we're
  // ready for the next message.

  messageInputBox.value = "";
  messageInputBox.focus();
}

// Callback for when the STUN server responds with the ICE candidates.
function onIceCandidate(event) {
  if (event && event.candidate) {
    webSocketConnection.send(
      JSON.stringify({ messageType: "candidate", payload: event.candidate })
    );
    console.log("Candiadte: ");
    console.log(event.candidate);
  }
}

// Callback for when the SDP offer was successfully created.
function onOfferFulfilled(description) {
  console.log("before", description.sdp);
  //   description.sdp = preferSelectedCodec(description.sdp);
  //   console.log("after", description.sdp);
  rtcPeerConnection.setLocalDescription(description);
  webSocketConnection.send(
    JSON.stringify({ messageType: "offer", payload: description })
  );
}

//callback when receive remote track
function onTrack(event) {
  if (event.streams && event.streams[0]) {
    console.log("ONTRACK");
    console.log(event.streams[0]);
    remotevideo.srcObject = event.streams[0];
    remotevideo.play();
  } else {
    console.log("No stream");
  }
}

function onOfferRejected(reason) {
  console.log(reason);
}

// Callback for when the WebSocket is successfully opened.
function onWebSocketOpen() {
  // NOTE: Two stun servers, with two different ip addresses binded
  // to different tcp ports are needed in the general case
  // https://stackoverflow.com/questions/7594390/why-a-stun-server-needs-two-different-public-ip-addresses
  const config = {
    iceServers: [
      {
        urls: [
          "stun:stun1.l.google.com:19302",
          "stun:stun2.l.google.com:19305",
        ],
      },
    ],
  };
  rtcPeerConnection = new RTCPeerConnection(config);
  rtcPeerConnection.idx = 1;
  //For reference, check https://hpbn.co/webrtc/
  const dataChannelConfig = { ordered: false, maxRetransmits: 0 };

  dataChannel = rtcPeerConnection.createDataChannel("dc", dataChannelConfig);
  dataChannel.onmessage = onDataChannelMessage;
  dataChannel.onopen = onDataChannelOpen;
  rtcPeerConnection.ondatachannel = (event) => (dataChannel = event.channel);

  dataChannel.onerror = onDataChannelError;

  rtcPeerConnection.ondatachannel = onDataChannelOpen;
  rtcPeerConnection.onicecandidate = onIceCandidate;

  rtcPeerConnection.oniceconnectionstatechange = function (event) {
    console.log("State change: " + rtcPeerConnection.iceConnectionState);
  };

  rtcPeerConnection.onaddstream = (event) => {
    console.log("check", rtcPeerConnection);
    console.log("ONADDSTREAM");
    console.log(event);
    remotevideo.srcObject = event.stream;
    remotevideo.play();
  };
  // localStream.getVideoTracks.length

  //rtcPeerConnection.ontrack=onTrack;

  // rtcPeerConnection.addStream(localStream);
  rtcPeerConnection.addTransceiver("video", { direction: "recvonly" });
  rtcPeerConnection.addTransceiver("audio", { direction: "recvonly" });

  rtcPeerConnection
    .createOffer({
      OfferToReceiveAudio: true,
      OfferToReceiveVideo: true,
    })
    .then(onOfferFulfilled, onOfferRejected);

  console.log("Peer conection state: " + rtcPeerConnection.connectionState);

  // rtcPeerConnection.addTrack(localStream.getVideoTracks()[0]);

  // rtcPeerConnection.addEventListener('track',function(event){
  //     console.log("received track remote");
  //     remotevideo.srcObject = event.streams[0];
  //     });

  getStats(
    rtcPeerConnection,
    function (result) {
      previewGetStatsResult(rtcPeerConnection, result);
    },
    1000
  );
}

function startMedia() {
  var promisifiedOldGUM = function (
    constraints,
    successCallback,
    errorCallback
  ) {
    // First get ahold of getUserMedia, if present
    var getUserMedia =
      navigator.getUserMedia ||
      navigator.webkitGetUserMedia ||
      navigator.mozGetUserMedia;

    // Some browsers just don't implement it - return a rejected promise with an error
    // to keep a consistent interface
    if (!getUserMedia) {
      return Promise.reject(
        new Error("getUserMedia is not implemented in this browser")
      );
    }

    // Otherwise, wrap the call to the old navigator.getUserMedia with a Promise
    return new Promise(function (successCallback, errorCallback) {
      getUserMedia.call(navigator, constraints, successCallback, errorCallback);
    });
  };

  // Older browsers might not implement mediaDevices at all, so we set an empty object first
  if (navigator.mediaDevices === undefined) {
    navigator.mediaDevices = {};
  }

  // Some browsers partially implement mediaDevices. We can't just assign an object
  // with getUserMedia as it would overwrite existing properties.
  // Here, we will just add the getUserMedia property if it's missing.
  if (navigator.mediaDevices.getUserMedia === undefined) {
    navigator.mediaDevices.getUserMedia = promisifiedOldGUM;
  }

  // Prefer camera resolution nearest to 1280x720.
  var constraints = { audio: true, video: true };

  navigator.mediaDevices
    .getUserMedia(constraints)
    .then(function (stream) {
      //stream.getVideoTracks()[0].enabled=false;
      // stream.getAudioTracks()[0].enabled=false;
      console.log("VideoTracks length: \t" + stream.getVideoTracks().length);

      // localStream = stream;
      try {
        // if(stream.getVideoTracks().length!=0){
        //     // localvid.srcObject=stream;
        //     // localvid.play();
        // }else{
        //     console.log(
        //         "Null video"
        //     );
        // }
      } catch (e) {
        console.log("Error setting video src: ", e);
      }
    })
    .catch(function (err) {
      console.log(err.name + ": " + err.message);
      if (location.protocol === "http:") {
        alert("Please test this using HTTPS.");
      } else {
        alert("Have you enabled the appropriate flag? see README.md");
      }
      console.error(e);
    });
}

// stop local video
function stopMedia() {
  // localvid.src = "";
  // localStream.getVideoTracks()[0].stop();
}

// Callback for when we receive a message from the server via the WebSocket.
function onWebSocketMessage(event) {
  console.log("OnWebSocketMessage: ");
  const messageObject = JSON.parse(event.data);
  console.log("payload: " + messageObject.payload);
  if (messageObject.messageType === "answer") {
    console.log("Setting remote description");

    rtcPeerConnection.setRemoteDescription(
      new RTCSessionDescription(messageObject.payload)
    );
  } else if (messageObject.messageType === "candidate") {
    rtcPeerConnection.addIceCandidate(
      new RTCIceCandidate(messageObject.payload)
    );
  }
}

// Connects by creating a new WebSocket connection and associating some callbacks.
function connect(hostname, port, usehttps) {
  let protoPrefix = usehttps ? "wss://" : "ws://";
  let webSocketUrl = protoPrefix + hostname + ":" + port + "/";
  webSocketConnection = new WebSocket(webSocketUrl);
  // webSocketConnection = new WebSocket("ws://127.0.0.1:9002");
  // webSocketConnection.on
  webSocketConnection.onopen = onWebSocketOpen;
  webSocketConnection.onmessage = onWebSocketMessage;

  webSocketConnection.addEventListener("message", function (event) {
    console.log("Message from server ", event.data);
  });
}

var STOP_GETSTATS = false;

function previewGetStatsResult(peer, result) {
  console.log(result);
  if (STOP_GETSTATS) {
    result.nomore();
    return;
  }

  if (result.connectionType.remote.candidateType.indexOf("relayed") !== -1) {
    result.connectionType.remote.candidateType = "TURN";
  } else {
    result.connectionType.remote.candidateType = "STUN";
  }

  document.getElementById(
    "peer" + peer.idx + "-totalDataForReceivers"
  ).innerHTML = bytesToSize(
    result.audio.bytesReceived + result.video.bytesReceived
  );

  document.getElementById("peer" + peer.idx + "-audio-latency").innerHTML =
    result.audio.latency + "ms";
  document.getElementById("peer" + peer.idx + "-video-latency").innerHTML =
    result.video.latency + "ms";

  document.getElementById("peer" + peer.idx + "-audio-packetsLost").innerHTML =
    result.audio.packetsLost;
  document.getElementById("peer" + peer.idx + "-video-packetsLost").innerHTML =
    result.video.packetsLost;
  document.getElementById("peer" + peer.idx + "-codecsSend").innerHTML =
    result.audio.send.codecs.concat(result.video.send.codecs).join(", ");
  document.getElementById("peer" + peer.idx + "-codecsRecv").innerHTML =
    result.audio.recv.codecs.concat(result.video.recv.codecs).join(", ");
  if (result.ended === true) {
    result.nomore();
  }

  window.getStatsResult = result;
}

function bytesToSize(bytes) {
  var k = 1000;
  var sizes = ["Bytes", "KB", "MB", "GB", "TB"];
  if (bytes <= 0) {
    return "0 Bytes";
  }
  var i = parseInt(Math.floor(Math.log(bytes) / Math.log(k)), 10);

  if (!sizes[i]) {
    return "0 Bytes";
  }

  return (bytes / Math.pow(k, i)).toPrecision(3) + " " + sizes[i];
}

var codec = "h264";

function preferSelectedCodec(sdp) {
  console.log("orignal", sdp);
  var info = splitLines(sdp);
  var newsdp = "";
  info.sdp.forEach(function (s) {
    newsdp += s;
    newsdp += "\n";
  });
  newsdp = newsdp.slice(0, -1);

  // if (
  //     codec === "vp8" &&
  //     info.vp8LineNumber === info.videoCodecNumbers[0]
  // ) {
  //     return sdp;
  // }

  // if (
  //     codec === "vp9" &&
  //     info.vp9LineNumber === info.videoCodecNumbers[0]
  // ) {
  //     return sdp;
  // }

  // if (
  //     codec === "h264" &&
  //     info.h264LineNumber === info.videoCodecNumbers[0]
  // ) {
  //     return sdp;
  // }

  // sdp = preferCodec(sdp, codec, info);

  return newsdp;
}

function preferCodec(sdp, codec, info) {
  var preferCodecNumber = "";

  // if (codec === "vp8") {
  //     if (!info.vp8LineNumber) {
  //         return sdp;
  //     }
  //     preferCodecNumber = info.vp8LineNumber;
  // }

  // if (codec === "vp9") {
  //     if (!info.vp9LineNumber) {
  //         return sdp;
  //     }
  //     preferCodecNumber = info.vp9LineNumber;
  // }

  // if (codec === "h264") {
  //     if (!info.h264LineNumber) {
  //         return sdp;
  //     }

  //     preferCodecNumber = info.h264LineNumber;
  // }

  // var newLine =
  //     info.videoCodecNumbersOriginal.split("SAVPF")[0] + "SAVPF ";

  // var newOrder = [preferCodecNumber];
  // info.videoCodecNumbers.forEach(function (codecNumber) {
  //     if (codecNumber === preferCodecNumber) return;
  //     newOrder.push(codecNumber);
  // });

  // newLine += newOrder.join(" ");

  // sdp = sdp.replace(info.videoCodecNumbersOriginal, newLine);
  return info;
}

function splitLines(sdp) {
  var info = {};
  info.sdp = [];
  sdp.split("\n").forEach(function (line) {
    if (line.indexOf("m=video") === 0) {
      info.sdp.push(
        line.replace(line.split("SAVPF")[1], " 102 127 125 108 124 123")
      );
    } else if (line.indexOf("a=rtpmap:96") == 0) {
    } else if (line.indexOf("a=rtcp-fb:96") == 0) {
    } else if (line.indexOf("a=rtpmap:97") == 0) {
    } else if (line.indexOf("a=rtcp-fb:97") == 0) {
    } else if (line.indexOf("a=rtpmap:98") == 0) {
    } else if (line.indexOf("a=rtcp-fb:98") == 0) {
    } else if (line.indexOf("a=rtpmap:99") == 0) {
    } else if (line.indexOf("a=rtcp-fb:99") == 0) {
    } else if (line.indexOf("a=rtpmap:100") == 0) {
    } else if (line.indexOf("a=rtcp-fb:100") == 0) {
    } else if (
      line.indexOf("a=rtpmap:101") == 0 ||
      line.indexOf("a=rtpmap:107") == 0
    ) {
    } else if (line.indexOf("a=rtcp-fb:101") == 0) {
    } else if (line.indexOf("a=rtpmap:122") == 0) {
    } else if (line.indexOf("a=rtcp-fb:122") == 0) {
    } else if (line.indexOf("a=rtpmap:121") == 0) {
    } else if (line.indexOf("a=rtcp-fb:121") == 0) {
    } else if (
      line.indexOf("a=rtpmap:35") == 0 ||
      line.indexOf("a=rtpmap:36") == 0
    ) {
    } else if (
      line.indexOf("a=rtcp-fb:35") == 0 ||
      line.indexOf("a=rtcp-fb:36") == 0
    ) {
    } else if (line.indexOf("a=rtpmap:109") == 0) {
    } else if (line.indexOf("a=rtcp-fb:109") == 0) {
    } else if (line.indexOf("a=rtpmap:120") == 0) {
    } else if (line.indexOf("a=rtcp-fb:120") == 0) {
    } else if (line.indexOf("a=rtpmap:119") == 0) {
    } else if (line.indexOf("a=rtcp-fb:119") == 0) {
    } else if (line.indexOf("a=rtpmap:118") == 0) {
    } else if (line.indexOf("a=rtcp-fb:118") == 0) {
    } else if (line.indexOf("a=rtpmap:114") == 0) {
    } else if (line.indexOf("a=rtcp-fb:114") == 0) {
    } else if (line.indexOf("a=rtpmap:115") == 0) {
    } else if (line.indexOf("a=rtcp-fb:115") == 0) {
    } else if (line.indexOf("a=rtpmap:116") == 0) {
    } else if (line.indexOf("a=rtcp-fb:116") == 0) {
    } else if (line.indexOf("a=rtpmap:37") == 0) {
    } else if (line.indexOf("a=rtcp-fb:37") == 0) {
    } else if (
      line.indexOf("a=fmtp:96") == 0 ||
      line.indexOf("a=fmtp:37") == 0 ||
      line.indexOf("a=fmtp:116") == 0 ||
      line.indexOf("a=fmtp:115") == 0
    ) {
    } else if (
      line.indexOf("a=fmtp:114") == 0 ||
      line.indexOf("a=fmtp:118") == 0 ||
      line.indexOf("a=fmtp:119") == 0 ||
      line.indexOf("a=fmtp:35") == 0
    ) {
    } else if (
      line.indexOf("a=fmtp:109") == 0 ||
      line.indexOf("a=fmtp:107") == 0 ||
      line.indexOf("a=fmtp:120") == 0 ||
      line.indexOf("a=fmtp:121") == 0
    ) {
    } else if (
      line.indexOf("a=fmtp:101") == 0 ||
      line.indexOf("a=fmtp:100") == 0 ||
      line.indexOf("a=fmtp:99") == 0 ||
      line.indexOf("a=fmtp:98") == 0
    ) {
    } else if (
      line.indexOf("a=fmtp:97") == 0 ||
      line.indexOf("a=fmtp:36") == 0 ||
      line.indexOf("a=fmtp:122") == 0
    ) {
    } else {
      info.sdp.push(line);
    }
    // if (line.indexOf("VP8/90000") !== -1 && !info.vp8LineNumber) {
    //     info.vp8LineNumber = line.replace("a=rtpmap:", "").split(" ")[0];
    // }

    // if (line.indexOf("VP9/90000") !== -1 && !info.vp9LineNumber) {
    //     info.vp9LineNumber = line.replace("a=rtpmap:", "").split(" ")[0];
    // }

    // if (line.indexOf("H264/90000") !== -1 && !info.h264LineNumber) {
    //     info.h264LineNumber = line.replace("a=rtpmap:", "").split(" ")[0];
    // }
  });

  return info;
}
