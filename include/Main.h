#pragma once

#include <stdexcept>
#include <cstdio>
#include "Decoder.h"
#include "Bus.h"
#include "CPU.h"
#include "GPU.h"
#include "FrameBuffer.h"
#include "ProgramUtils.h"

#define WIDTH 320
#define HEIGHT 240 
#define FRAME_BUFFER_SIZE (WIDTH * HEIGHT * 4)
#define MEMORY_SIZE 4194304

void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i != 0) printf(" ");
    }
    printf("\n");
}
