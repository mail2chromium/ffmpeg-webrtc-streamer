// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "video_frame_buffer.h"
#include "vector"

class H264FrameBuffer : public webrtc::VideoFrameBuffer
{
public:
    H264FrameBuffer(int Width, int Height)
            : Width(Width)
            , Height(Height)
    {
    }

    //
    // webrtc::VideoFrameBuffer interface
    //
    Type type() const override
    {
        return Type::kNative;
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
