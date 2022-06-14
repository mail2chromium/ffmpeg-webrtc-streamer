// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "../src/WebRTCProxyCommon.h"
#include "SharedQueue.h"

// Forward declarations
struct FClientSession;

class IVideoEncoderObserver
{
public:
	virtual void ForceKeyFrame() = 0;
	virtual void SetRate(uint32_t BitrateKbps, uint32_t Framerate) = 0;
};

class FVideoEncoder : public webrtc::VideoEncoder
{
public:
//	explicit FVideoEncoder(IVideoEncoderObserver& Observer, FClientSession& OwnerSession);
	explicit FVideoEncoder(/*IVideoEncoderObserver& Observer*/);

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
	FClientSession* OwnerSession = nullptr;
	webrtc::EncodedImageCallback* Callback = nullptr;
	webrtc::EncodedImage EncodedImage;
	//std::vector<uint8_t> EncodedImageBuffer;
	std::vector<uint8_t> EncodedImageBuffer;
	webrtc::H264BitstreamParser BitstreamParser;
	webrtc::CodecSpecificInfo CodecSpecific;
	webrtc::RTPFragmentationHeader FragHeader;
	bool bStartedFromSPS = false;
	size_t FrameNo = 0;

	std::atomic<bool> bOwnsQualityControl ;
	std::atomic<bool> bForceBitrateRequest ;
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
};

class FVideoEncoderFactory : public webrtc::VideoEncoderFactory
{
public:
	explicit FVideoEncoderFactory(/*IVideoEncoderObserver& videoSource*/);

	/**
	 * This is used from the FClientSession::OnSucess to let the factory know
	 * what session the next created encoder should belong to.
	 * It allows us to get the right FClientSession <-> FVideoEncoder relationship
	 */
	void AddSession(FClientSession& ClientSession);

	//
	// webrtc::VideoEncoderFactory implementation
	//


	std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
	CodecInfo QueryVideoEncoder(const webrtc::SdpVideoFormat& format) const override;
	std::unique_ptr<webrtc::VideoEncoder> CreateVideoEncoder(const webrtc::SdpVideoFormat& format) override;

private:
	IVideoEncoderObserver* VideoSource;
	TSharedQueue<FClientSession*> PendingClientSessions;
};
