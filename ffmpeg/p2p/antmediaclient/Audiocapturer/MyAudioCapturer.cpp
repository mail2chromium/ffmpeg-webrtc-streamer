// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MyAudioCapturer.h"
#include "rtc_base/logging.h"
#include "audio/channel_send.h"

////// Encoder settings
const uint8_t kPayloadType = 111 & 0x7f;
int sample_rate = 48000;
size_t channels = 2;
int timestamp_ = 0;

////// SDP Formatting
const webrtc::SdpAudioFormat &audio_sdp_format = {"OPUS", sample_rate, channels};
rtc::BufferT<uint8_t> encoded;

////// Create built-in Encoder Factory
auto factory = webrtc::CreateBuiltinAudioEncoderFactory();
std::unique_ptr<webrtc::AudioEncoder> encoder = factory->MakeAudioEncoder(kPayloadType, audio_sdp_format, absl::nullopt);

// These are copied from webrtc internals
#define CHECKinitialized_() \
    {                       \
        if (!bInitialized)  \
        {                   \
            return -1;      \
        };                  \
    }
#define CHECKinitialized__BOOL() \
    {                            \
        if (!bInitialized)       \
        {                        \
            return false;        \
        };                       \
    }

static int16_t combine(std::uint8_t a, std::uint8_t b) {
    int16_t out = a << 8 | b;
    return out;
}

void MyAudioCapturer::ProcessPacket(
    // PixelStreamingProtocol::EToProxyMsg PkType,
    const void *Data, uint32_t Size, bool is_opus) {
    if (!(bInitialized)) {
        RTC_LOG(LS_ERROR) << "Returning ";
        return;
    }

    auto uint8_data = static_cast<const uint8_t *>(Data);
    int BytesPer10Ms = (SampleRate * Channels * static_cast<int>(sizeof(uint16_t))) / 100;

    wavFileReader::getinstance()->writeFile((uint8_t*) uint8_data, Size);

    if (is_opus) {
        //set encoded data and size
        webrtc::voe::ffmpeg_encoded_buffer_size = Size;
        for (int i = 0; i < Size; i++) {
            webrtc::voe::ffmpeg_encoded_buffer[i] = uint8_data[i];
        }
        const uint8_t *null_buff = new uint8_t[BytesPer10Ms]();
        auto PkData = static_cast<const uint8_t *>(null_buff);
        RecordingBuffer.insert(RecordingBuffer.end(), null_buff, null_buff + BytesPer10Ms);
        DeviceBuffer->SetRecordedBuffer(RecordingBuffer.data(), BytesPer10Ms / (sizeof(uint16_t) * Channels));
        DeviceBuffer->DeliverRecordedData();
        RecordingBuffer.erase(RecordingBuffer.begin(), RecordingBuffer.begin() + BytesPer10Ms);
        delete null_buff;

//    //TODO; insert data into ring buffer and fetch 480 samples
//    RingBuffer_In(ring_, uint8_data, Size);
//
//    uint8_t *in = new uint8_t[960];
//    if(RingBuffer_Len(ring_) >= 960) {
//        RingBuffer_Out(ring_, in, 960);
//    }
//
//    /////// Converting uint8_t array to int16_t array
//    int i = 0;
//    int16_t *out;
//    out = new int16_t [960 / 2];
//
//    int j = 0;
//    for (; i < 960; i += 2) {
//        out[j] = uint8_data[i] << 8 | uint8_data[i + 1];
//        j++;
//    }
//
//    ////// Encoding audio packets
//    encoder->Encode(timestamp_, rtc::ArrayView<const int16_t>(out, 480), &encoded);
//    timestamp_ += 480;
//
//    ////// Output payload (encoded)
//    const rtc::ArrayView<const uint8_t> opayload(encoded.data(), encoded.size());
//
//    std::string wav_out;
//    for (int j = 0; j < opayload.size(); j++) {
//        wav_out += std::to_string(opayload.data()[j]);
//        if (j != opayload.size() - 1)
//            wav_out += " ";
//    }
//    RTC_LOG(LS_ERROR) << encoded.size() << " external opus bytes: " << wav_out.c_str();
//
//    //set encoded data and size
//    webrtc::voe::ffmpeg_encoded_buffer_size = opayload.size();
//    for (int i = 0; i < opayload.size(); i++) {
//        webrtc::voe::ffmpeg_encoded_buffer[i] = opayload.data()[i];
//    }
//    const uint8_t *null_buff = new uint8_t[BytesPer10Ms]();
//    auto PkData = static_cast<const uint8_t *>(null_buff);
//
//    RecordingBuffer.insert(RecordingBuffer.end(), null_buff, null_buff + BytesPer10Ms);
//
//    DeviceBuffer->SetRecordedBuffer(RecordingBuffer.data(), BytesPer10Ms / (sizeof(uint16_t) * Channels));
//    DeviceBuffer->DeliverRecordedData();
//
//    RecordingBuffer.erase(RecordingBuffer.begin(), RecordingBuffer.begin() + BytesPer10Ms);
//
//    delete null_buff;
//    delete out;
//    encoded.Clear();

    }
    else {
        auto PkData = static_cast<const uint8_t *>(Data);
        RecordingBuffer.insert(RecordingBuffer.end(), PkData, PkData + Size);
        // Feed in 10ms chunks
        while (RecordingBuffer.size() >= BytesPer10Ms) {
            // If this check fails, then it means we tried to use it after it was
            // destroyed in "Terminate". If so, then we should use a mutex around it
            // so we are either destroying it, or using it.
            // The way the objects and threads interact, and the way shutdown is done,
            // it shouldn't happen, but nevertheless, having the check doesn't hurt.
            if (DeviceBuffer) {
                //            std::cout << "Buffer init..\n";
                //            RTC_LOG(LS_ERROR) << "bytesper10ms: " << BytesPer10Ms;
                //            RTC_LOG(LS_ERROR) << "Channels: " << Channels;
                //            RTC_LOG(LS_ERROR) << "Sample_per_channel: " << BytesPer10Ms / (sizeof(uint16_t) * Channels);

                DeviceBuffer->SetRecordedBuffer(RecordingBuffer.data(),
                                                BytesPer10Ms / (sizeof(uint16_t) * Channels));
//                    DeviceBuffer->SetVQEData(50, 50);
                DeviceBuffer->DeliverRecordedData();
            }

            RecordingBuffer.erase(RecordingBuffer.begin(), RecordingBuffer.begin() + BytesPer10Ms);
        }
    }
}

int32_t MyAudioCapturer::ActiveAudioLayer(AudioLayer *audioLayer) const {
    //LOGFUNC();
    *audioLayer = AudioDeviceModule::kDummyAudio;
    return 0;
}

int32_t MyAudioCapturer::RegisterAudioCallback(webrtc::AudioTransport *audioCallback) {
    //LOGFUNC();
    DeviceBuffer->RegisterAudioCallback(audioCallback);
    return 0;
}

int32_t MyAudioCapturer::Init() {
    //LOGFUNC();
    if (bInitialized)
        return 0;

    m_taskQueueFactory = webrtc::CreateDefaultTaskQueueFactory();
    DeviceBuffer = std::make_unique<webrtc::AudioDeviceBuffer>(m_taskQueueFactory.get());

    bInitialized = true;
    return 0;
}

int32_t MyAudioCapturer::Terminate() {
    //LOGFUNC();
    if (!bInitialized)
        return 0;

    DeviceBuffer.reset();

    bInitialized = false;
    return 0;
}

bool MyAudioCapturer::Initialized() const {
    //LOGFUNC();
    return bInitialized;
}

int16_t MyAudioCapturer::PlayoutDevices() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int16_t MyAudioCapturer::RecordingDevices() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::PlayoutDeviceName(
        uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::RecordingDeviceName(
        uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::SetPlayoutDevice(uint16_t index) {
    //LOGFUNC();
    CHECKinitialized_();
    return 0;
}

int32_t MyAudioCapturer::SetPlayoutDevice(WindowsDeviceType device) {
    //LOGFUNC();
    CHECKinitialized_();
    return 0;
}

int32_t MyAudioCapturer::SetRecordingDevice(uint16_t index) {
    //LOGFUNC();
    CHECKinitialized_();
    return 0;
}

int32_t MyAudioCapturer::SetRecordingDevice(WindowsDeviceType device) {
    //LOGFUNC();
    CHECKinitialized_();
    return 0;
}

int32_t MyAudioCapturer::PlayoutIsAvailable(bool *available) {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::InitPlayout() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

bool MyAudioCapturer::PlayoutIsInitialized() const {
    //LOGFUNC();
    CHECKinitialized__BOOL();
    return false;
}

int32_t MyAudioCapturer::RecordingIsAvailable(bool *available) {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::InitRecording() {
    CHECKinitialized_();

    // #Audio : Allow dynamic values for samplerate and/or channels ,
    // or receive those from UE4 ?
    DeviceBuffer->SetRecordingSampleRate(SampleRate);
    DeviceBuffer->SetRecordingChannels(Channels);

    bRecordingInitialized = true;
    return 0;
}

bool MyAudioCapturer::RecordingIsInitialized() const {
    //LOGFUNC();
    CHECKinitialized__BOOL();
    return bRecordingInitialized == true;
}

int32_t MyAudioCapturer::StartPlayout() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::StopPlayout() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

bool MyAudioCapturer::Playing() const {
    //LOGFUNC();
    CHECKinitialized__BOOL();
    return false;
}

int32_t MyAudioCapturer::StartRecording() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::StopRecording() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

bool MyAudioCapturer::Recording() const {
    //LOGFUNC();
    CHECKinitialized__BOOL();
    return bRecordingInitialized;
}

int32_t MyAudioCapturer::InitSpeaker() {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

bool MyAudioCapturer::SpeakerIsInitialized() const {
    //LOGFUNC();
    CHECKinitialized__BOOL();
    return false;
}

int32_t MyAudioCapturer::InitMicrophone() {
    //LOGFUNC();
    CHECKinitialized_();
    return 0;
}

bool MyAudioCapturer::MicrophoneIsInitialized() const {
    //LOGFUNC();
    CHECKinitialized__BOOL();
    return true;
}

int32_t MyAudioCapturer::StereoPlayoutIsAvailable(bool *available) const {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::SetStereoPlayout(bool enable) {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::StereoPlayout(bool *enabled) const {
    //LOGFUNC();
    CHECKinitialized_();
    return -1;
}

int32_t MyAudioCapturer::StereoRecordingIsAvailable(bool *available) const {
    //LOGFUNC();
    CHECKinitialized_();
    *available = true;
    return 0;
}

int32_t MyAudioCapturer::SetStereoRecording(bool enable) {
    //LOGFUNC();
    CHECKinitialized_();
    return 0;
}

int32_t MyAudioCapturer::StereoRecording(bool *enabled) const {
    //LOGFUNC();
    CHECKinitialized_();
    *enabled = true;
    return 0;
}
