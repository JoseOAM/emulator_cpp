#pragma once

#include "FrameBuffer.h"
#include "Memory.h"
#include "ComponentType.h"
#include "MemoryException.h"

class Bus {
public:
    Bus(FrameBuffer* frameBuffer, Memory* memory);
    virtual ~Bus() = default;

    void write(int address, int* value, int valueSize);
    ComponentType whichComponentType(int address);
    int* read(int address, int endDataPosition);

private:
    FrameBuffer* frameBuffer;
    Memory* memory;
    int frameBufferSize;
    int memorySize;
};
