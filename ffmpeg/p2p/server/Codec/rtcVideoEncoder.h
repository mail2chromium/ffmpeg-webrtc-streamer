// Copyright Ottoman 2022, Inc. All Rights Reserved.

#pragma once

#include <common_video/h264/h264_bitstream_parser.h>
#include <api/video/encoded_image.h>
#include <api/video_codecs/video_encoder.h>
#include <modules/video_coding/include/video_codec_interface.h>
#include <api/video_codecs/video_encoder_factory.h>


class IVideoEncoderObserver
{
public:
	virtual void ForceKeyFrame() = 0;
	virtual void SetRate(uint32_t BitrateKbps, uint32_t Framerate) = 0;
};

class CustomVideoEncoder : public webrtc::VideoEncoder
{
public:
	explicit CustomVideoEncoder(IVideoEncoderObserver& Observer);

	void SetQualityControlOwnership(bool bOwnership);
	bool HasQualityControlOwnership();

	//
	// webrtc::VideoEncoder interface
	//
	// Updated by Muhammad Usman
	/*
	Replacement of SetRates(uint32_t Bitrate, uint32_t Framerate) &
	InitEncode()
	*/
	int32_t InitEncode(const webrtc::VideoCodec* codecSettings, const VideoEncoder::Settings& settings) override;
	int32_t RegisterEncodeCompleteCallback(webrtc::EncodedImageCallback* Callback) override;
	int32_t Release() override;
	 int32_t Encode(
      const webrtc::VideoFrame& inputImage,
      const std::vector<webrtc::VideoFrameType>* frame_types) override;

	// Updated by Muhammad Usman.
	// Replacement of SetChannelParameters(uint32_t PacketLoss, int64_t Rtt)
	void OnRttUpdate(int64_t rtt_ms) override;
	void OnPacketLossRateUpdate(float packet_loss_rate) override;


	void SetRates(const RateControlParameters& parameters) override;
	void SetFecControllerOverride(webrtc::FecControllerOverride* fec_controller_override) override;
	 /*
	 Update by Muhammad Usman
	 Replacement of GetScalingSettings()

	 */
	EncoderInfo GetEncoderInfo() const override;
	ScalingSettings GetScalingSettings() const;
	bool SupportsNativeHandle() const;

private:
	IVideoEncoderObserver* Observer;

	// Client session that this encoder instance belongs to
	webrtc::EncodedImageCallback* Callback = nullptr;
	webrtc::EncodedImage EncodedImage;
	//std::vector<uint8_t> EncodedImageBuffer;
	std::vector<uint8_t> EncodedImageBuffer;
	webrtc::H264BitstreamParser BitstreamParser;
	webrtc::CodecSpecificInfo CodecSpecific;
	webrtc::RTPFragmentationHeader FragHeader;
	bool bStartedFromSPS = false;
	size_t FrameNo = 0;
//
//	std::atomic<bool> bOwnsQualityControl = false;
//	std::atomic<bool> bForceBitrateRequest = false;
	webrtc::VideoBitrateAllocation LastBitrate;
	uint32_t LastFramerate = 0;

	// Updated by Muhammad Usman
	/*
	Replacement of SetRates(uint32_t Bitrate, uint32_t Framerate) &
	SetRateAllocation(const webrtc::BitrateAllocation& Allocation, uint32_t Framerate)

	*/
	RateControlParameters rate_control_parameters;
	/*
	Update by Muhammad Usman
	Replacement of GetScalingSettings()

	*/
	//EncoderInfo encoder_info;
    std::unique_ptr<CustomVideoEncoder> video_encoder;

};


class FVideoEncoderFactory : public webrtc::VideoEncoderFactory
{
public:
    explicit FVideoEncoderFactory(IVideoEncoderObserver& videoSource);

/**
     * This is used from the FClientSession::OnSucess to let the factory know
     * what session the next created encoder should belong to.
     * It allows us to get the right FClientSession <-> CustomVideoEncoder relationship
     */

    //
    // webrtc::VideoEncoderFactory implementation
    //


    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
    CodecInfo QueryVideoEncoder(const webrtc::SdpVideoFormat& format) const override;
    std::unique_ptr<webrtc::VideoEncoder> CreateVideoEncoder(const webrtc::SdpVideoFormat& format) override;

private:
    IVideoEncoderObserver* VideoSource;
};

