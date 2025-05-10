#ifndef __Decoder_h
#define __Decoder_h

#include <cstdio>
#include <cassert>

static char* decodeUType(int instruction) {

    int opcode = instruction & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int imm = instruction >> 12;
    char* operation = (char*) malloc(64 * sizeof(char));

    if (operation != NULL) {
        switch (opcode) {
        case 0x37:
        {
            snprintf(operation, 64 * sizeof(char), "lui rd=%d, imm=%d", rd, imm);
            break;
        }
        case 0x17:
        {
            snprintf(operation, 64 * sizeof(char), "auipc rd=%d, imm=%d", rd, imm);
            break;
        }
        default:
            snprintf(operation, 64 * sizeof(char), "Unknown");
            break;
        }
    }

    return operation;
}

static char* decodeJType(int instruction) {

    int imm20 = (instruction >> 31) & 0x1;
    int imm10_1 = (instruction >> 21) & 0x3FF;
    int imm11 = (instruction >> 20) & 0x1;
    int imm19_12 = (instruction >> 12) & 0xFF;

    int opcode = instruction & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);

    char* operation = (char*) malloc(64 * sizeof(char));
    if (operation != NULL) {
        snprintf(operation, 64 * sizeof(char), "jal rd=%d, imm=%d", rd, imm);
    }

    return operation;
}

static char* decodeInstruction(int instruction) {

    int opcode = instruction & 0x7F;
    char* operation = NULL;

    switch (opcode) {
    case 0x33:
    {
        printf("Instruction R-Type %d\n", instruction);
        break;
    };
    case 0x67:
    {
        printf("Instruction I-Type-jalr %d\n", instruction);
        break;
    };
    case 0x03:
    {
        printf("Instruction I-Type-load %d\n", instruction);
        break;
    };
    case 0x13:
    {
        printf("Instruction I-Type-imm %d\n", instruction);
        break;
    };
    case 0x73:
    {
        printf("Instruction I-Type-csr %d\n", instruction);
        break;
    };
    case 0x23:
    {
        printf("Instruction S-Type %d\n", instruction);
        break;
    };
    case 0x63:
    {
        printf("Instruction B-Type %d\n", instruction);
        break;
    };
    case 0x37:
    case 0x17:
    {
        operation = decodeUType(instruction);
        break;
    };
    case 0x6F:
    {
        operation = decodeJType(instruction);
        break;
    };
    default:
    {
        printf("Unknown Type\n");
        char msg[256];
        snprintf(msg, sizeof(msg), "Unknown operation: %d", opcode);
        throw std::runtime_error(msg);
        break;
    };
    };

    return operation;
}

#endif