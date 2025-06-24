#include "Memory.h"

Memory::Memory(int memorySize) {
	this->memory = new uint8_t[memorySize];
	this->memorySize = memorySize;
}

Memory::~Memory() {
	free(this->memory);
}

void Memory::write(int beginDataPosition, uint8_t* value, int valueSize) {
    if (beginDataPosition < 0 || beginDataPosition + valueSize > memorySize) {
        throw MemoryException("Memory overflow at position %d", beginDataPosition + valueSize);
    }

    for (int i = 0; i < valueSize; ++i) {
        memory[beginDataPosition + i] = value[i];
    }
}

void Memory::writeFromInt(int beginDataPosition, int* value, int valueSize) {
    int length = valueSize * 4;

    if (beginDataPosition < 0 || beginDataPosition + length > memorySize) {
        throw MemoryException("Memory overflow at position %d", beginDataPosition + length);
    }

    for (int i = 0; i < valueSize; ++i) {
        int v = value[i];
        memory[beginDataPosition + i * 4 + 0] = static_cast<uint8_t>((v >> 24) & 0xFF);
        memory[beginDataPosition + i * 4 + 1] = static_cast<uint8_t>((v >> 16) & 0xFF);
        memory[beginDataPosition + i * 4 + 2] = static_cast<uint8_t>((v >> 8) & 0xFF);
        memory[beginDataPosition + i * 4 + 3] = static_cast<uint8_t>((v >> 0) & 0xFF);
    }
}

uint8_t* Memory::read(int beginDataPosition, int endDataPosition) {
    if (beginDataPosition < 0 || endDataPosition > memorySize) {
        throw MemoryException("Invalid range specified.");
    }
    if (endDataPosition <= beginDataPosition) {
        throw MemoryException("End position must be greater than begin position");
    }

    int length = endDataPosition - beginDataPosition;
    uint8_t* result = new uint8_t[length];

    std::memcpy(result, memory + beginDataPosition, length);
    return result;
}

int* Memory::readAsInt(int beginDataPosition, int endDataPosition) {
    if (beginDataPosition < 0 || endDataPosition > memorySize) {
        throw MemoryException("Invalid range specified");
    }
    if (endDataPosition <= beginDataPosition) {
        throw MemoryException("End position must be greater than begin position");
    }
    int length = endDataPosition - beginDataPosition;
    if (length % 4 != 0) {
        throw MemoryException("Invalid range specified");
    }

    int numInts = length / 4;
    int* intArray = new int[numInts];

    for (int i = 0; i < numInts; ++i) {
        uint8_t b0 = memory[beginDataPosition + i * 4 + 0];
        uint8_t b1 = memory[beginDataPosition + i * 4 + 1];
        uint8_t b2 = memory[beginDataPosition + i * 4 + 2];
        uint8_t b3 = memory[beginDataPosition + i * 4 + 3];

        int value = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
        intArray[i] = value;
    }

    return intArray;
}
