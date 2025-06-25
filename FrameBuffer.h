#pragma once

#include "ByteBuffer.h"
#include "MemoryException.h"

struct RenderDataDto {
    float* vertex;
    float* pixel;
    int length;

    RenderDataDto(float* v, float* p, int len)
        : vertex(v), pixel(p), length(len) {
    }
};

class FrameBuffer {
public:
    FrameBuffer(int bufferSize);
    ~FrameBuffer();

    int getFrameBufferSize();
    void checkAddressSize(int beginAddress, int data);
    void checkAddressRange(int beginAddress, int data);
    ByteBuffer getByteBufferFromBuffer(uint8_t* buffer, int beginAddress, int endAddress);
    void writePixel(int beginAddress, int* data, int valueSize);
    void writeToVertexBufferFromFloats(int beginAddress, float* data, int dataSize);
    void writeToPixelBufferFromFloats(int beginAddress, float* data, int dataSize);
    void writeToBufferFromFloats(uint8_t* buffer, int beginAddress, float* data, int dataSize);
    void swap();
    RenderDataDto getRenderData();
    float* readFromPixelBufferAsFloats(int beginAddress, int endAddress);
    float* readFromVertexBufferAsFloats(int beginAddress, int endAddress);
    float* readFromBufferAsFloats(uint8_t* buffer, int beginAddress, int endAddress);
    int* readFromPixelBufferAsInts(int beginAddress, int endAddress);

private:
    int bufferSize;
    uint8_t* frontPixelBuffer;
    uint8_t* backPixelBuffer;
    uint8_t* frontVertexBuffer;
    uint8_t* backVertexBuffer;
};
