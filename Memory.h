#pragma once

#include <stdio.h>
#include <cstdint>
#include <stdexcept>
#include "MemoryException.h"

class Memory {
public:
    Memory(int memorySize);
    ~Memory();

    void write(int beginDataPosition, uint8_t* value, int valueSize);
    void writeFromInt(int beginDataPosition, int* value, int valueSize);
    uint8_t* read(int beginDataPosition, int endDataPosition);
    int* readAsInt(int beginDataPosition, int endDataPosition);

private:
    int memorySize;
    uint8_t* memory;
};
