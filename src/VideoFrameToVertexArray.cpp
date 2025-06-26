#include "VideoFrameToVertexArray.h"

VideoFrameToVertexArray::VideoFrameToVertexArray(const char* path, int w, int h, Bus* b, FrameBuffer* fb)
    : videoFilePath(path), width(w), height(h), bus(b), frameBuffer(fb), shouldStop(false), resizedFrameBuffer(nullptr)
{
    resizedFrameBuffer = new uint8_t[width * height * 4];
}

VideoFrameToVertexArray::~VideoFrameToVertexArray() {
    stop();
    if (resizedFrameBuffer) {
        delete[] resizedFrameBuffer;
        resizedFrameBuffer = nullptr;
    }
}

void VideoFrameToVertexArray::start() {
    shouldStop = false;
    workerThread = std::thread(&VideoFrameToVertexArray::run, this);
}

void VideoFrameToVertexArray::stop() {
    shouldStop = true;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void VideoFrameToVertexArray::resizeRGBAImage(
    const uint8_t* srcData, int srcWidth, int srcHeight,
    uint8_t* dstData, int dstWidth, int dstHeight)
{
    for (int y = 0; y < dstHeight; ++y) {
        int srcY = y * srcHeight / dstHeight;
        for (int x = 0; x < dstWidth; ++x) {
            int srcX = x * srcWidth / dstWidth;

            int srcIndex = (srcY * srcWidth + srcX) * 4;
            int dstIndex = (y * dstWidth + x) * 4;

            for (int c = 0; c < 4; ++c) {
                dstData[dstIndex + c] = srcData[srcIndex + c];
            }
        }
    }
}

void VideoFrameToVertexArray::run() {
    avformat_network_init();

    AVFormatContext* formatContext = nullptr;
    if (avformat_open_input(&formatContext, videoFilePath, nullptr, nullptr) != 0) {
        throw std::runtime_error("Failed to open video file.");
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        avformat_close_input(&formatContext);
        throw std::runtime_error("Failed to find stream info.");
    }

    int videoStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }
    if (videoStreamIndex == -1) {
        avformat_close_input(&formatContext);
        throw std::runtime_error("No video stream found.");
    }

    AVCodecParameters* codecParams = formatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        avformat_close_input(&formatContext);
        throw std::runtime_error("Codec not found.");
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        avformat_close_input(&formatContext);
        throw std::runtime_error("Failed to allocate codec context.");
    }

    if (avcodec_parameters_to_context(codecContext, codecParams) < 0) {
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        throw std::runtime_error("Failed to copy codec parameters.");
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        throw std::runtime_error("Failed to open codec.");
    }

    AVFrame* frame = av_frame_alloc();
    AVFrame* frameRGBA = av_frame_alloc();
    if (!frame || !frameRGBA) {
        av_frame_free(&frame);
        av_frame_free(&frameRGBA);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        throw std::runtime_error("Failed to allocate frames.");
    }

    int srcWidth = codecContext->width;
    int srcHeight = codecContext->height;

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, srcWidth, srcHeight, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) {
        av_frame_free(&frame);
        av_frame_free(&frameRGBA);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        throw std::runtime_error("Failed to allocate buffer.");
    }

    av_image_fill_arrays(frameRGBA->data, frameRGBA->linesize, buffer, AV_PIX_FMT_RGBA, srcWidth, srcHeight, 1);

    SwsContext* swsCtx = sws_getContext(srcWidth, srcHeight, codecContext->pix_fmt,
        srcWidth, srcHeight, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsCtx) {
        av_free(buffer);
        av_frame_free(&frame);
        av_frame_free(&frameRGBA);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        throw std::runtime_error("Failed to initialize sws context.");
    }

    AVPacket packet;
    AVPacket* pkt = av_packet_alloc();
    if (!pkt) {
        throw std::runtime_error("Failed to allocate AVPacket");
    }

    while (!shouldStop) {
        if (av_read_frame(formatContext, &packet) < 0) {
            av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
            continue;
        }

        if (packet.stream_index == videoStreamIndex) {
            int ret = avcodec_send_packet(codecContext, &packet);
            if (ret < 0) {
                av_packet_unref(&packet);
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(codecContext, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                }
                else if (ret < 0) {
                    av_packet_unref(&packet);
                    goto cleanup;
                }

                sws_scale(swsCtx, frame->data, frame->linesize, 0, srcHeight,
                    frameRGBA->data, frameRGBA->linesize);

                resizeRGBAImage(frameRGBA->data[0], srcWidth, srcHeight, resizedFrameBuffer, width, height);

                int address = 4100;
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        int idx = (y * width + x) * 4;
                        uint32_t pixel = (resizedFrameBuffer[idx + 3] << 24) |
                            (resizedFrameBuffer[idx + 0] << 16) |
                            (resizedFrameBuffer[idx + 1] << 8) |
                            (resizedFrameBuffer[idx + 2]);
                        bus->write(address, (int*)&pixel, 1);
                        address += 4;
                    }
                }
                int zeroVal[1] = { 0 };
                bus->write(0x100000, zeroVal, 1);

                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }
        av_packet_unref(&packet);
    }

cleanup:
    sws_freeContext(swsCtx);
    av_free(buffer);
    av_frame_free(&frame);
    av_frame_free(&frameRGBA);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    avformat_network_deinit();
    av_packet_free(&pkt);
}
