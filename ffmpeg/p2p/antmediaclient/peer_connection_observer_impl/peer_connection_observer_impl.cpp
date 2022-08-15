//
// Created by zain on 11/5/2021.
//

#include <nlohmann/json.hpp>
#include "peer_connection_observer_impl.h"
#include "../WebSocket/WebSocketHelper.h"
#include <test/video_renderer.h>

//headers for write video to file
//#include <Windows.h>
//#include <mfapi.h>
//#include <mfidl.h>
//#include <Mfreadwrite.h>
//#include <mferror.h>
//
//#pragma comment(lib, "mfreadwrite")
//#pragma comment(lib, "mfplat")
//#pragma comment(lib, "mfuuid")

//template for write video to file
//template<class T>
//void SafeRelease(T **ppT) {
//    if (*ppT) {
//        (*ppT)->Release();
//        *ppT = NULL;
//    }
//}


//datachannelobserverimpl data_channel_observer;
using json = nlohmann::json;

websocketpp::connection_hdl websocket_connection_handler;
websocketserver ws_server;
client AntMediaWSTrasport;
rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track;


// Format constants for write video to file
//const UINT32 VIDEO_WIDTH = 640;
//const UINT32 VIDEO_HEIGHT = 480;
//const UINT32 VIDEO_FPS = 30;
//const UINT64 VIDEO_FRAME_DURATION = 10 * 1000 * 1000 / VIDEO_FPS;
//const UINT32 VIDEO_BIT_RATE = 800000;
//const GUID VIDEO_ENCODING_FORMAT = MFVideoFormat_WMV3;
//const GUID VIDEO_INPUT_FORMAT = MFVideoFormat_RGB32;
//const UINT32 VIDEO_PELS = VIDEO_WIDTH * VIDEO_HEIGHT;
//const UINT32 VIDEO_FRAME_COUNT = 20 * VIDEO_FPS;
//DWORD videoFrameBuffer[VIDEO_PELS];

void peerconnectionobserverimpl::OnIceCandidate(const webrtc::IceCandidateInterface *candidate) {
    //TODO;    sending the iceCandidates
    //    {
    //        command : "takeCandidate",
    //            streamId : "stream1",
    //                label : "${CANDIDATE.SDP_MLINE_INDEX}",
    //                id : "${CANDIDATE.SDP_MID}",
    //                candidate : "${CANDIDATE.CANDIDATE}"
    //    }

    std::string candidate_str;
    candidate->ToString(&candidate_str);
    RTC_LOG(LS_NONE) << "oniceCandidate called " << candidate_str;
    json message_object;
    message_object["command"] = "takeCandidate";
    message_object["streamId"] = LOCAL_STREAM_ID;
    message_object["candidate"] = candidate_str;
    message_object["id"] = candidate->sdp_mid();
    message_object["label"] = candidate->sdp_mline_index();
    AntMediaWSTrasport.send(websocket_connection_handler, message_object.dump(),
                            websocketpp::frame::opcode::value::text);

}

//write video to file
/*
HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex) {
    *ppWriter = NULL;
    *pStreamIndex = NULL;

    IMFSinkWriter *pSinkWriter = NULL;
    IMFMediaType *pMediaTypeOut = NULL;
    IMFMediaType *pMediaTypeIn = NULL;
    DWORD streamIndex;

    HRESULT hr = MFCreateSinkWriterFromURL(L"output.wmv", NULL, NULL, &pSinkWriter);

    // Set the output media type.
    if (SUCCEEDED(hr)) {
        hr = MFCreateMediaType(&pMediaTypeOut);
    }
    if (SUCCEEDED(hr)) {
        hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }
    if (SUCCEEDED(hr)) {
        hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, VIDEO_ENCODING_FORMAT);
    }
    if (SUCCEEDED(hr)) {
        hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, VIDEO_BIT_RATE);
    }
    if (SUCCEEDED(hr)) {
        hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }
    if (SUCCEEDED(hr)) {
        hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);
    }
    if (SUCCEEDED(hr)) {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, VIDEO_FPS, 1);
    }
    if (SUCCEEDED(hr)) {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    }
    if (SUCCEEDED(hr)) {
        hr = pSinkWriter->AddStream(pMediaTypeOut, &streamIndex);
    }

    // Set the input media type.
    if (SUCCEEDED(hr)) {
        hr = MFCreateMediaType(&pMediaTypeIn);
    }
    if (SUCCEEDED(hr)) {
        hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }
    if (SUCCEEDED(hr)) {
        hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, VIDEO_INPUT_FORMAT);
    }
    if (SUCCEEDED(hr)) {
        hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }
    if (SUCCEEDED(hr)) {
        hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);
    }
    if (SUCCEEDED(hr)) {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, VIDEO_FPS, 1);
    }
    if (SUCCEEDED(hr)) {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    }
    if (SUCCEEDED(hr)) {
        hr = pSinkWriter->SetInputMediaType(streamIndex, pMediaTypeIn, NULL);
    }

    // Tell the sink writer to start accepting data.
    if (SUCCEEDED(hr)) {
        hr = pSinkWriter->BeginWriting();
    }

    // Return the pointer to the caller.
    if (SUCCEEDED(hr)) {
        *ppWriter = pSinkWriter;
        (*ppWriter)->AddRef();
        *pStreamIndex = streamIndex;
    }

    SafeRelease(&pSinkWriter);
    SafeRelease(&pMediaTypeOut);
    SafeRelease(&pMediaTypeIn);
    return hr;
}

HRESULT WriteFrame(IMFSinkWriter *pWriter, DWORD streamIndex, const LONGLONG &rtStart) {
    IMFSample *pSample = NULL;
    IMFMediaBuffer *pBuffer = NULL;

    const LONG cbWidth = 4 * VIDEO_WIDTH;
    const DWORD cbBuffer = cbWidth * VIDEO_HEIGHT;

    BYTE *pData = NULL;

    // Create a new memory buffer.
    HRESULT hr = MFCreateMemoryBuffer(cbBuffer, &pBuffer);

    // Lock the buffer and copy the video frame to the buffer.
    if (SUCCEEDED(hr)) {
        hr = pBuffer->Lock(&pData, NULL, NULL);
    }
    if (SUCCEEDED(hr)) {
        hr = MFCopyImage(
                pData,                      // Destination buffer.
                cbWidth,                    // Destination stride.
                (BYTE *) videoFrameBuffer,    // First row in source image.
                cbWidth,                    // Source stride.
                cbWidth,                    // Image width in bytes.
                VIDEO_HEIGHT                // Image height in pixels.
        );
    }
    if (pBuffer) {
        pBuffer->Unlock();
    }

    // Set the data length of the buffer.
    if (SUCCEEDED(hr)) {
        hr = pBuffer->SetCurrentLength(cbBuffer);
    }

    // Create a media sample and add the buffer to the sample.
    if (SUCCEEDED(hr)) {
        hr = MFCreateSample(&pSample);
    }
    if (SUCCEEDED(hr)) {
        hr = pSample->AddBuffer(pBuffer);
    }

    // Set the time stamp and the duration.
    if (SUCCEEDED(hr)) {
        hr = pSample->SetSampleTime(rtStart);
    }
    if (SUCCEEDED(hr)) {
        hr = pSample->SetSampleDuration(VIDEO_FRAME_DURATION);
    }

    // Send the sample to the Sink Writer.
    if (SUCCEEDED(hr)) {
        hr = pWriter->WriteSample(streamIndex, pSample);
    }

    SafeRelease(&pSample);
    SafeRelease(&pBuffer);
    return hr;
}

void writeFile() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr)) {
            IMFSinkWriter *pSinkWriter = NULL;
            DWORD stream;

            hr = InitializeSinkWriter(&pSinkWriter, &stream);
            if (SUCCEEDED(hr)) {
                // Send frames to the sink writer.
                LONGLONG rtStart = 0;

                for (DWORD i = 0; i < VIDEO_FRAME_COUNT; ++i) {
                    hr = WriteFrame(pSinkWriter, stream, rtStart);
                    if (FAILED(hr)) {
                        break;
                    }
                    rtStart += VIDEO_FRAME_DURATION;
                }
            }
            if (SUCCEEDED(hr)) {
                hr = pSinkWriter->Finalize();
            }
            SafeRelease(&pSinkWriter);
            MFShutdown();
        }
        CoUninitialize();
    }
}

void writeVideoFile() {

    WSASession Session;
    UDPSocket Socket;
    Socket.Bind(8080);

    int sampleSize = 61400;
    uint8_t *sample = new uint8_t[sampleSize + 1]; //one extra for sequence Number at start
    int bufLen = 921600; // 640 * 480 *3
    uint8_t *tuppleBuffer = new uint8_t[bufLen];
    int tupleBufIndex = 0;
    int sampleRecved = 0;
    int framesRecv = 0;


    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr)) {
            IMFSinkWriter *pSinkWriter = NULL;
            DWORD stream;

            hr = InitializeSinkWriter(&pSinkWriter, &stream);
            if (SUCCEEDED(hr)) {
                // Send frames to the sink writer.
                LONGLONG rtStart = 0;

                //receive frames here

                int lastSeqNumber = 1;
                bool newFrame = true;
                while (framesRecv < 250) {
                    Socket.RecvFrom(reinterpret_cast<char *>(sample), sampleSize + 1);

                    int seqNumber = (int) sample[0];

                    if (newFrame) {
                        lastSeqNumber = seqNumber;
                        newFrame = false;
                    }
                    if (seqNumber != lastSeqNumber) {
                        //sequence number changed , new Frame started
                        newFrame = true;
                        lastSeqNumber = seqNumber;

                        //convert data to UL
                        int index = 0;
                        for (int l = 0; l < bufLen;) {
                            unsigned long ul =
                                    tuppleBuffer[l++] | (tuppleBuffer[l++] << 8) | (tuppleBuffer[l++] << 16);
                            videoFrameBuffer[index] = ul;
                            index++;
                        }

                        RTC_LOG(LS_NONE) << "\nSize of VideoFrameBuffer: " << sizeof(videoFrameBuffer) ;
                        sampleRecved = 0;
                        ++framesRecv;
                        tupleBufIndex = 0;
                        RTC_LOG(LS_NONE) << "Frames recv: " << framesRecv ;

                        //write frame to file

                        hr = WriteFrame(pSinkWriter, stream, rtStart);
                        if (FAILED(hr)) {
                            break;
                        }
                        rtStart += VIDEO_FRAME_DURATION;


                    }
                    if (seqNumber == lastSeqNumber) {
                        //same sequence number as last sample , goes to same frame
                        for (int i = 1; i <= sampleSize; ++i) {
                            tuppleBuffer[tupleBufIndex++] = sample[i];
                        }
                    }

                    RTC_LOG(LS_NONE) << "sampleRecv: " << ++sampleRecved << ", Seq Num: " << seqNumber ;

                }

            }
            if (SUCCEEDED(hr)) {
                hr = pSinkWriter->Finalize();
            }
            SafeRelease(&pSinkWriter);
            MFShutdown();
        }
        CoUninitialize();
    }
}

void receiveFromOpenCV() {

    std::vector<uint8_t> bufferVector;
    H264FrameBuffer frameBuffer(640, 480);
    frameBuffer.SetBuffer(&bufferVector);
    try {

        writeVideoFile();

    }
    catch (std::system_error &e) {
        RTC_LOG(LS_NONE) << e.code();
    }
}

[[noreturn]] void receiveVideoFromObs() {

    string filename = "Local_OBS_video";
    FILE *fp = fopen((string("d:\\") + filename + ".yuv").c_str(), "w");
    int itr = 0;
    int LastNtpTimeMs = 0;

    auto *s = new
            DatagramSocket(8080, "0.0.0.0", TRUE, TRUE);
    auto *t = new
            MyThread(s);
    std::vector<uint8_t> vector_;
    H264FrameBuffer buffer(1920, 1080);
    buffer.SetBuffer(&vector_);

    int resolution = 1920 * 1080;
    int pixels = resolution * 3;
    auto *recvData = new uint8_t[pixels];

    int num = 0;
    while (true) {
        s->receive(recvData, pixels);
        for (int i = 0; i < pixels; i++) {
            vector_.push_back(recvData[i]);
            //RTC_LOG(LS_NONE) << recvData[i];
        }
        RTC_LOG(LS_NONE) << "Frame num : " << num++ ;
        vi
        deo_source->OnFrameCaptured(&buffer);


        //write data to file

//    int64_t NtpTimeMs = rtc::TimeMillis();
//    if (NtpTimeMs <= LastNtpTimeMs)
//        NtpTimeMs = LastNtpTimeMs + 1;
//    LastNtpTimeMs = NtpTimeMs;
//
//        webrtc::VideoFrame frame = webrtc::VideoFrame::Builder()
//        .set_video_frame_buffer(&buffer)
//        .set_rotation(webrtc::VideoRotation(0))
//        .set_timestamp_us(NtpTimeMs / rtc::kNumNanosecsPerMicrosec)
//        .build();
//
//         if (itr < 100) {
//        //webrtc::PrintVideoFrame(frame, fp);
//        itr++;
//        if (itr == 100) {
//            fclose(fp);
//            RTC_LOG(LS_NONE) << "Closed\n";
//            break;
//        }
//       // buffer.GetBuffer()->clear();
//    }

    }
}
*/
void peerconnectionobserverimpl::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
    cout << "ondatachannel";
    data_channel = channel;
//    data_channel->RegisterObserver(&data_channel_observer);
}

void peerconnectionobserverimpl::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) {
    if (new_state ==
        webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionMax) {
        cout << "Ice state max\n";
    } else if (new_state ==
               webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew) {
        cout << "Ice state new\n";

    } else if (new_state ==
               webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionChecking) {
        cout << "Ice state cheking\n";
    } else if (new_state ==
               webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionFailed) {
        cout << "Ice state failed\n";
    } else if (new_state ==
               webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected) {
        cout << "Ice state dis conceted\n";
    } else if (new_state ==
               webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionCompleted) {
        cout << "Ice state completdd\n";
    } else if (new_state ==
               webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionClosed) {
        cout << "Ice state closed\n";
    } else if (new_state ==
               webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionConnected) {
        cout << "Ice state conected\n";


    }
}


void peerconnectionobserverimpl::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
    RTC_LOG(LS_NONE) << "OnAddStream\n";
    video_track = stream->GetVideoTracks()[0];
    auto audio = stream->GetAudioTracks()[0];
    audio.get()->set_enabled(true);
    RTC_LOG(LS_ERROR) << "Audio track: " << ToString(audio.get());
    RTC_LOG(LS_ERROR) << "Video track: " << ToString(video_track.get());


    auto video_renderer
            = webrtc::test::VideoRenderer::Create("AntMedia: rtp_stream", 640, 480);
    video_track->AddOrUpdateSink(video_renderer, rtc::VideoSinkWants());

}

void peerconnectionobserverimpl::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) {
    RTC_LOG(LS_NONE) << "Received track remote\n";
    //PeerConnectionObserver::OnTrack(transceiver);
}