// Copyright Ottoman 2022, Inc. All Rights Reserved.

#include "MyAudioCapturer.h"
#include "rtc_base/logging.h"

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

void MyAudioCapturer::ProcessPacket(
        // PixelStreamingProtocol::EToProxyMsg PkType,
        const void *Data, uint32_t Size) {

    RTC_LOG(LS_ERROR) << "Processing packet ";
//	if (PkType != PixelStreamingProtocol::EToProxyMsg::AudioPCM)
//	{
//		return;
//	}

    if (!(bInitialized)) {
        RTC_LOG(LS_ERROR) << "Returning ";
        return;
    }

    RTC_LOG(LS_ERROR) << "Going to init Pkdata ";

    auto PkData = static_cast<const uint8_t *>(Data);

    RecordingBuffer.insert(RecordingBuffer.end(), PkData, PkData + Size);

    int BytesPer10Ms = (SampleRate * Channels * static_cast<int>(sizeof(uint16_t))) / 100;
//    int BytesPer10Ms = (SampleRate * Channels * static_cast<int>(sizeof(uint16_t))) / 400;

    // Feed in 10ms chunks
    while (RecordingBuffer.size() >= BytesPer10Ms) {
        // If this check fails, then it means we tried to use it after it was
        // destroyed in "Terminate". If so, then we should use a mutex around it
        // so we are either destroying it, or using it.
        // The way the objects and threads interact, and the way shutdown is done,
        // it shouldn't happen, but nevertheless, having the check doesn't hurt.
        if (DeviceBuffer) {
            std::cout << "Buffer init..\n";
            RTC_LOG(LS_ERROR) << "bytesper10ms: " << BytesPer10Ms;
            RTC_LOG(LS_ERROR) << "Channels: " << Channels;
            RTC_LOG(LS_ERROR) << "Sample_per_channel: " << BytesPer10Ms / (sizeof(uint16_t) * Channels);

            DeviceBuffer->SetRecordedBuffer(RecordingBuffer.data(), BytesPer10Ms / (sizeof(uint16_t) * Channels));
//            DeviceBuffer->SetVQEData(total_delay_ms_, 0);
            DeviceBuffer->DeliverRecordedData();
        }

        RecordingBuffer.erase(RecordingBuffer.begin(), RecordingBuffer.begin() + BytesPer10Ms);
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
