#pragma once

#include "Bus.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdint>

class ProgramUtils {
public:
    ProgramUtils(Bus* bus);
    ~ProgramUtils();

    void setUtils(Bus* bus);
    int* readFile(const char* path, int* fileSize);
    void writeProgramInMemory(int* programBin, int programSize);

private:
    Bus* bus;

    int* readBinaryFile(std::ifstream& file, int* fileSize);
    int* readTxtFile(std::ifstream& file, int* fileSize);
};
