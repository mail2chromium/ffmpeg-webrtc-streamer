/**
 * This a minimal fully functional example for setting up a client written in JavaScript that
 * communicates with a server via WebRTC data channels. This uses WebSockets to perform the WebRTC
 * handshake (offer/accept SDP) with the server. We only use WebSockets for the initial handshake
 * because TCP often presents too much latency in the context of real-time action games. WebRTC
 * data channels, on the other hand, allow for unreliable and unordered message sending via SCTP
 *
 * Brian Ho
 * brian@brkho.com
 */

// The WebSocket object used to manage a connection.
let webSocketConnection = null;
// The RTCPeerConnection through which we engage in the SDP handshake.
let rtcPeerConnection = null;
// The data channel used to communicate.
let dataChannel = null;

var sendButton= document.getElementById('sendButton');

// var localvid=document.getElementById('localvideo');
var remotevideo=document.getElementById('remotevideo');
var localStream=null;
// sendButton.addEventListener('click', sendMessageThroughDataChannel, false);

var messageInputBox = document.getElementById('message');

// Callback for when we receive a message on the data channel.
function onDataChannelMessage(event) {
    console.log('DataChannelMessage');
    console.log(event.data)
    document.getElementById("receiveMsg").innerHTML = event.data
}



// Called when we are not the caller (ie we are the receiver)
// and the data channel has been opened
function handleDataChannelCreated(event) {
    console.log('dataChannel opened');

    dataChannel = event.channel;
    dataChannel.onmessage = handleDataChannelReceiveMessage;
    dataChannel.onopen = handleDataChannelStatusChange;
    dataChannel.onclose = handleDataChannelStatusChange;
}

function onDataChannelOpen(event) {
    console.log('DataChannelOpen');
    //dataChannel.send("PING");
}

function onDataChannelError(error) {
    console.log('DataChannel Error : ' + error);
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
        webSocketConnection.send(JSON.stringify({ messageType: 'candidate', payload: event.candidate }));
        console.log("Candiadte: ")
        console.log(event.candidate)
    }
}

// Callback for when the SDP offer was successfully created.
function onOfferFulfilled(description) {
    rtcPeerConnection.setLocalDescription(description);
    webSocketConnection.send(JSON.stringify({ messageType: 'offer', payload: description }));
}

//callback when receive remote track
function onTrack(event){

    if(event.streams && event.streams[0]){



        console.log("ONTRACK");
        console.log(event);
        console.log(event.streams[0]);
        remotevideo.srcObject=event.streams[0];
        remotevideo.play();
    }else{
        console.log("No stream");
    }
}

function onOfferRejected(reason) {
    console.log(reason)
}

// Callback for when the WebSocket is successfully opened.
function onWebSocketOpen() {
    // NOTE: Two stun servers, with two different ip addresses binded
    // to different tcp ports are needed in the general case
    // https://stackoverflow.com/questions/7594390/why-a-stun-server-needs-two-different-public-ip-addresses
    const config = { iceServers: [{ urls: ["stun:stun1.l.google.com:19302", "stun:stun2.l.google.com:19305" ] }] };
    rtcPeerConnection = new RTCPeerConnection(config);
    //For reference, check https://hpbn.co/webrtc/
    const dataChannelConfig = { ordered: false, maxRetransmits: 0 };

    dataChannel = rtcPeerConnection.createDataChannel('dc', dataChannelConfig);
    dataChannel.onmessage = onDataChannelMessage;
    dataChannel.onopen = onDataChannelOpen;
    rtcPeerConnection.ondatachannel = event => dataChannel = event.channel;

    dataChannel.onerror = onDataChannelError;

    rtcPeerConnection.ondatachannel = onDataChannelOpen
    rtcPeerConnection.onicecandidate = onIceCandidate;

    rtcPeerConnection.oniceconnectionstatechange = function(event){
        console.log("State change: "+rtcPeerConnection.iceConnectionState);    
    }


    rtcPeerConnection.onaddstream=(event)=>{
console.log('check',rtcPeerConnection);
      console.log("ONADDSTREAM");
      console.log(event);
        const audioTracks = event.stream.getAudioTracks();
        console.log("AudioTrack : ",audioTracks)
        event.stream.getTracks().forEach(track => console.log("each track: ",track, event.stream));

        remotevideo.srcObject=event.stream;
      remotevideo.play();

    }
    // localStream.getVideoTracks.length
   
   //rtcPeerConnection.ontrack=onTrack;

    // rtcPeerConnection.addStream(localStream);
    rtcPeerConnection.addTransceiver("video", { direction: "recvonly" });
    rtcPeerConnection.addTransceiver("audio", { direction: "recvonly" });
    
    rtcPeerConnection.createOffer( {
        OfferToReceiveAudio: true,
        OfferToReceiveVideo: true
      }).then(onOfferFulfilled, onOfferRejected);

    console.log("Peer conection state: "+rtcPeerConnection.connectionState);

    // rtcPeerConnection.addTrack(localStream.getVideoTracks()[0]);

    // rtcPeerConnection.addEventListener('track',function(event){
    //     console.log("received track remote");
    //     remotevideo.srcObject = event.streams[0];
    //     });

}

function startMedia() {

    var promisifiedOldGUM = function(constraints, successCallback, errorCallback) {

        // First get ahold of getUserMedia, if present
        var getUserMedia = (navigator.getUserMedia ||
            navigator.webkitGetUserMedia ||
            navigator.mozGetUserMedia);

        // Some browsers just don't implement it - return a rejected promise with an error
        // to keep a consistent interface
        if(!getUserMedia) {
            return Promise.reject(new Error('getUserMedia is not implemented in this browser'));
        }

        // Otherwise, wrap the call to the old navigator.getUserMedia with a Promise
        return new Promise(function(successCallback, errorCallback) {
            getUserMedia.call(navigator, constraints, successCallback, errorCallback);
        });
    }

    // Older browsers might not implement mediaDevices at all, so we set an empty object first
    if(navigator.mediaDevices === undefined) {
        navigator.mediaDevices = {};
    }

    // Some browsers partially implement mediaDevices. We can't just assign an object
    // with getUserMedia as it would overwrite existing properties.
    // Here, we will just add the getUserMedia property if it's missing.
    if(navigator.mediaDevices.getUserMedia === undefined) {
        navigator.mediaDevices.getUserMedia = promisifiedOldGUM;
    }

    // Prefer camera resolution nearest to 1280x720.
    var constraints = { audio: true, video: true };

    navigator.mediaDevices.getUserMedia(constraints)
        .then(function(stream) {
             //stream.getVideoTracks()[0].enabled=false;
            // stream.getAudioTracks()[0].enabled=false;
            console.log("VideoTracks length: \t"+ stream.getVideoTracks().length);

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

            } catch(e) {
                console.log("Error setting video src: ", e);
            }
        })
        .catch(function(err) {
            console.log(err.name + ": " + err.message);
            if (location.protocol === 'http:') {
                alert('Please test this using HTTPS.');
            } else {
                alert('Have you enabled the appropriate flag? see README.md');
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
    console.log("payload: "+messageObject.payload);
    if (messageObject.messageType === 'answer') {
        console.log("Setting remote description");
        rtcPeerConnection.setRemoteDescription(new RTCSessionDescription(messageObject.payload));
    } else if (messageObject.messageType === 'candidate') {
        rtcPeerConnection.addIceCandidate(new RTCIceCandidate(messageObject.payload));

    }
}

// Connects by creating a new WebSocket connection and associating some callbacks.
function connect(hostname, port, usehttps)
{
    let protoPrefix = usehttps ? "wss://" : "ws://";
    let webSocketUrl = protoPrefix + hostname + ":" + port + "/";
    webSocketConnection = new WebSocket(webSocketUrl);
    // webSocketConnection = new WebSocket("ws://127.0.0.1:9002");
    // webSocketConnection.on
    webSocketConnection.onopen = onWebSocketOpen;
    webSocketConnection.onmessage = onWebSocketMessage;

    webSocketConnection.addEventListener('message',function(event){
        console.log('Message from server ', event.data);
    });
}
