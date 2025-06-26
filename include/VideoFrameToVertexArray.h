#pragma once

#include <thread>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <cstring>
#include "Bus.h"
#include "FrameBuffer.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoFrameToVertexArray {
public:
    VideoFrameToVertexArray(const char* videoFilePath, int width, int height, Bus* bus, FrameBuffer* frameBuffer);
    ~VideoFrameToVertexArray();

    void start();
    void stop();

private:
    void run();
    static void resizeRGBAImage(
        const uint8_t* srcData, int srcWidth, int srcHeight,
        uint8_t* dstData, int dstWidth, int dstHeight);

    const char* videoFilePath;
    int width;
    int height;
    Bus* bus;
    FrameBuffer* frameBuffer;

    std::atomic<bool> shouldStop;
    std::thread workerThread;

    uint8_t* resizedFrameBuffer;

    VideoFrameToVertexArray(const VideoFrameToVertexArray&) = delete;
    VideoFrameToVertexArray& operator=(const VideoFrameToVertexArray&) = delete;
};
