#pragma once

#include <exception>
#include <cstdio>
#include <cstdarg>

class MemoryException : public std::exception {
private:
    char message[256];

public:
    MemoryException(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        std::vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);
    }

    const char* what() const noexcept override {
        return message;
    }
};
