#include "Bus.h"

Bus::Bus(FrameBuffer* frameBuffer, Memory* memory) {
	this->frameBuffer = frameBuffer;
	this->memory = memory;
	this->frameBufferSize = frameBuffer->getFrameBufferSize();
	this->memorySize = memory->getMemorySize();
}

void Bus::write(int address, int* value, int valueSize) {
    ComponentType componentType = whichComponentType(address);

    switch (componentType) {
    case ComponentType::FRAME_BUFFER: {
        int frameBufferAddress = address - this->memorySize;

        if (frameBufferAddress >= 0 && frameBufferAddress <= 3) {
            this->frameBuffer->swap();
        }
        else {
            this->frameBuffer->writePixel(frameBufferAddress - 4, value, valueSize);
        }
        break;
    }

    case ComponentType::MEMORY:
        this->memory->writeFromInt(address, value, valueSize);
        break;

    default:
        throw MemoryException("Invalid component type for address: %d", address);
    }
}

ComponentType Bus::whichComponentType(int address) {
    if (address >= 0 && address < this->memorySize) {
        return ComponentType::MEMORY;
    }
    else if (address >= this->memorySize && address <= this->memorySize + this->frameBufferSize) {
        return ComponentType::FRAME_BUFFER;
    }
    else {
        throw MemoryException("Invalid address: %d", address);
    }
}

int* Bus::read(int address, int endDataPosition) {
    ComponentType componentType = whichComponentType(address);

    switch (componentType) {
    case ComponentType::FRAME_BUFFER: {
        int frameBufferAddress = address - this->memorySize - 4;
        int frameBufferEnd = endDataPosition - this->memorySize - 4;

        if (frameBufferAddress < 0) {
            throw MemoryException("Invalid address");
        }

        return this->frameBuffer->readFromPixelBufferAsInts(frameBufferAddress, frameBufferEnd);
    }

    case ComponentType::MEMORY:
        return this->memory->readAsInt(address, endDataPosition);

    default:
        throw MemoryException("Invalid component type");
    }
}
