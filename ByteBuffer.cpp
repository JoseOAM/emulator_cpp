#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(uint8_t* base, size_t len)
    : data(base), size(len), position(0) {
}

void ByteBuffer::setPosition(size_t pos) {
    if (pos > size) throw std::out_of_range("Position out of bounds");
    position = pos;
}

size_t ByteBuffer::getPosition() const { return position; }

int ByteBuffer::readInt() {
    if (position + 4 > size) throw std::out_of_range("Read past end");
    int value = (data[position] << 24) |
        (data[position + 1] << 16) |
        (data[position + 2] << 8) |
        (data[position + 3]);
    position += 4;
    return value;
}

float ByteBuffer::readFloat() {
    if (position + 4 > size) {
        throw std::out_of_range("Attempted to read beyond buffer size");
    }

    float value;
    std::memcpy(&value, &data[position], sizeof(float));
    position += 4;
    return value;
}
