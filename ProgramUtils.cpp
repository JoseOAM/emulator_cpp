#include "ProgramUtils.h"

ProgramUtils::ProgramUtils(Bus* bus) {
    this->bus = bus;
}

ProgramUtils::~ProgramUtils() {
}

void ProgramUtils::setUtils(Bus* bus) {
	this->bus = bus;
}

int* ProgramUtils::readFile(const char* path, int* outSize) {
    const char* extension = std::strrchr(path, '.');
    if (extension == nullptr) {
        throw std::invalid_argument("File must have .bin or .txt extension.");
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file.");
    }

    if (std::strcmp(extension, ".bin") == 0) {
        return this->readBinaryFile(file, outSize);
    }
    else if (std::strcmp(extension, ".txt") == 0) {
        return this->readTxtFile(file, outSize);
    }
    else {
        throw std::invalid_argument("File must have .bin or .txt extension.");
    }
}

int* ProgramUtils::readBinaryFile(std::ifstream& file, int* outSize) {
    if (!file.is_open()) {
        throw std::runtime_error("File not found");
    }

    file.seekg(0, std::ios::end);
    std::streampos sizeInBytes = file.tellg();
    file.seekg(0, std::ios::beg);

    if (sizeInBytes % 4 != 0) {
        throw std::runtime_error("The file size is not a multiple of 4 bytes, so it cannot be read as 32-bit integers.");
    }

    int numInts = static_cast<int>(sizeInBytes / 4);
    int* programBin = new int[numInts];

    uint8_t buffer[4];

    for (int i = 0; i < numInts; ++i) {
        file.read(reinterpret_cast<char*>(buffer), 4);
        if (file.gcount() != 4) {
            delete[] programBin;
            throw std::runtime_error("Error reading the binary file.");
        }

        programBin[i] =
            ((buffer[3] & 0xFF) << 24) |
            ((buffer[2] & 0xFF) << 16) |
            ((buffer[1] & 0xFF) << 8) |
            (buffer[0] & 0xFF);
    }

    *outSize = numInts;
    return programBin;
}

int* ProgramUtils::readTxtFile(std::ifstream& file, int* fileSize) {
    if (!file.is_open()) {
        throw std::runtime_error("File not found");
    }

    const int MAX_LINES = 1024;
    const int MAX_LINE_LENGTH = 128;

    char line[MAX_LINE_LENGTH];
    int* program = new int[MAX_LINES];
    int index = 0;

    while (file.getline(line, MAX_LINE_LENGTH)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\r') {
            line[len - 1] = '\0';
        }

        if (line[0] == '\0') continue;

        char* endptr;
        long long value = std::strtoll(line, &endptr, 2);

        if (*endptr != '\0') {
            delete[] program;
            throw std::runtime_error("Invalid binary line.");
        }

        program[index++] = static_cast<int>(value);
    }

    *fileSize = index;
    return program;
}

void ProgramUtils::writeProgramInMemory(int* programBin, int programSize) {
    this->bus->write(0, programBin, programSize);
    delete[] programBin;
}
