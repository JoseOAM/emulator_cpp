#pragma once

#include <cstdint>
#include <stdexcept>

class ByteBuffer {
private:
    uint8_t* data;
    size_t size;
    size_t position;

public:
    ByteBuffer(uint8_t* base, size_t len);

    void setPosition(size_t pos);
    size_t getPosition() const;
    int readInt();
    float readFloat();
};
