#pragma once

#include <stdexcept>
#include <cstdio>
#include "CPU.cpp"
#include "Bus.h"
#include "FrameBuffer.h"
#include "Decoder.h"

void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i != 0) printf(" ");
    }
    printf("\n");
}
