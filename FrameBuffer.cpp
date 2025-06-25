#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int bufferSize) {
    int size = bufferSize * 8;
    this->bufferSize = bufferSize * 2;
    this->frontPixelBuffer = new uint8_t[size]();
    this->backPixelBuffer = new uint8_t[size]();
    this->frontVertexBuffer = new uint8_t[size]();
    this->backVertexBuffer = new uint8_t[size]();
}

FrameBuffer::~FrameBuffer() {
    delete[] this->frontPixelBuffer;
    delete[] this->backPixelBuffer;
    delete[] this->frontVertexBuffer;
    delete[] this->backVertexBuffer;
}

int FrameBuffer::getFrameBufferSize() {
    return this->bufferSize;
}

void FrameBuffer::checkAddressSize(int beginAddress, int data) {
    int endAddress = beginAddress + data;
    int bufferLength = this->bufferSize * 4;

    if (beginAddress < 0 || endAddress > bufferLength || beginAddress >= endAddress) {
        throw MemoryException("Invalid data positions or data length. (beginAddress: %d, endAddress: %d)", beginAddress, endAddress);
    }
}

void FrameBuffer::checkAddressRange(int beginAddress, int endAddress) {
    int bufferLength = this->bufferSize * 4;

    if (beginAddress < 0 || endAddress > bufferLength || beginAddress >= endAddress) {
        throw MemoryException("Invalid data positions or data length. (beginAddress: %d, endAddress: %d)", beginAddress, endAddress);
    }
}

ByteBuffer FrameBuffer::getByteBufferFromBuffer(uint8_t* buffer, int beginAddress, int endAddress) {
    this->checkAddressRange(beginAddress, endAddress);

    ByteBuffer byteBuffer(buffer, this->bufferSize * 4);
    byteBuffer.setPosition(beginAddress);
    return byteBuffer;
}

void FrameBuffer::writePixel(int beginAddress, int* data, int dataSize) {
    int width = 320; // GPU.getWidth();
    int height = 240; //GPU.getHeight();

    for (int i = 0; i < dataSize; ++i) {
        int color = data[i];

        int x = ((beginAddress / 4) + (i * 4)) % width;
        int y = ((beginAddress / 4) + (i * 4)) / width;

        float normX = (static_cast<float>(x) / width) * 2.0f - 1.0f;
        float normY = (static_cast<float>(height - y) / height) * 2.0f - 1.0f;

        float r = ((color >> 16) & 0xFF) / 255.0f;
        float g = ((color >> 8) & 0xFF) / 255.0f;
        float b = (color & 0xFF) / 255.0f;
        float u = static_cast<float>(x) / width;
        float v = static_cast<float>(y) / height;

        float pixelFloats[4] = { r, g, b, 1.0f };
        this->writeToPixelBufferFromFloats(beginAddress * 4, pixelFloats, 4);

        float vertexFloats[8] = { normX, normY, r, g, b, 1.0f, u, v };
        this->writeToVertexBufferFromFloats((y * width + x) * 32, vertexFloats, 8);
    }
}

void FrameBuffer::writeToVertexBufferFromFloats(int beginAddress, float* data, int dataSize) {
    this->writeToBufferFromFloats(this->backVertexBuffer, beginAddress, data, dataSize);
}

void FrameBuffer::writeToPixelBufferFromFloats(int beginAddress, float* data, int dataSize) {
    this->writeToBufferFromFloats(this->backPixelBuffer, beginAddress, data, dataSize);
}

void FrameBuffer::writeToBufferFromFloats(uint8_t* buffer, int beginAddress, float* data, int dataSize) {
    this->checkAddressSize(beginAddress, dataSize * 4);

    for (int i = 0; i < dataSize; ++i) {
        float value = data[i];
        std::memcpy(buffer + beginAddress + i * 4, &value, 4);
    }
}

void FrameBuffer::swap() {
    uint8_t* temp = this->frontVertexBuffer;
    frontVertexBuffer = backVertexBuffer;
    backVertexBuffer = temp;

    temp = this->frontPixelBuffer;
    frontPixelBuffer = backPixelBuffer;
    backPixelBuffer = temp;

}

RenderDataDto FrameBuffer::getRenderData() {
    float* vertexData = this->readFromVertexBufferAsFloats(0, this->bufferSize);
    float* pixelData = this->readFromPixelBufferAsFloats(0, this->bufferSize);

    return RenderDataDto(vertexData, pixelData, this->bufferSize);
}

float* FrameBuffer::readFromPixelBufferAsFloats(int beginAddress, int endAddress) {
    return this->readFromBufferAsFloats(this->frontPixelBuffer, beginAddress, endAddress);
}

float* FrameBuffer::readFromVertexBufferAsFloats(int beginAddress, int endAddress) {
    return this->readFromBufferAsFloats(this->frontVertexBuffer, beginAddress, endAddress);

}

float* FrameBuffer::readFromBufferAsFloats(uint8_t* buffer, int beginAddress, int endAddress) {
    int length = endAddress - beginAddress;

    ByteBuffer byteBuffer = getByteBufferFromBuffer(buffer, beginAddress, endAddress);

    int floatCount = length / 4;
    float* floatArray = new float[floatCount];

    for (int i = 0; i < floatCount; ++i) {
        floatArray[i] = byteBuffer.readFloat();
    }

    return floatArray;
}

int* FrameBuffer::readFromPixelBufferAsInts(int beginAddress, int endAddress) {
    int length = endAddress - beginAddress;

    ByteBuffer byteBuffer = getByteBufferFromBuffer(this->frontPixelBuffer, beginAddress, endAddress);

    int count = length / 4;
    int* intArray = new int[count];

    for (int i = 0; i < count; ++i) {
        intArray[i] = byteBuffer.readInt();
    }

    return intArray;
}
