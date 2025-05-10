#ifndef __CPU_h
#define __CPU_h

static int signExtendImmediate(int immediate, int bitWidth) {
    if (bitWidth <= 0 || bitWidth >= 32) {
        return immediate;
    }

    int mask = 1 << (bitWidth - 1);
    immediate = immediate & ((1 << bitWidth) - 1);

    if ((immediate & mask) != 0) {
        immediate |= ~((1 << bitWidth) - 1);
    }
    return immediate;
}

#endif