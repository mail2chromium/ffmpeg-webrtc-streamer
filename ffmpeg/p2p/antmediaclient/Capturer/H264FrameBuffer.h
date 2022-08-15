// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

//#include "WebRTCProxyCommon.h"
#include "video_frame_buffer.h"
#include <vector>

class FH264FrameBuffer : public webrtc::VideoFrameBuffer
{
public:
	FH264FrameBuffer(int Width, int Height)
	    : Width(Width)
	    , Height(Height)
	{
	}

	//
	// webrtc::VideoFrameBuffer interface
	//
	Type type() const override
	{
		return Type::kI420;
	}

	virtual int width() const override
	{
		return Width;
	}

	virtual int height() const override
	{
		return Height;
	}


	rtc::scoped_refptr<webrtc::I420BufferInterface> ToI420() override
	{
//		check(false);
        RTC_LOG(LS_ERROR) << "ToI420() failed";
		return nullptr;
	}

	//
	// Own methods
	//
	std::vector<uint8_t>& GetBuffer()
	{
		return Buffer;
	}


private:
	int Width;
	int Height;
	std::vector<uint8_t> Buffer;
};
