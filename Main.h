#ifndef __Main_h
#define __Main_h

#include <stdexcept>
#include <cstdio>

int* registers = new int[32]();
int programCounter = 0;

void static executeUType(int instruction) {

    for (int i = 0; i < 32; i++) {
        printf("Register[%d] = %d\n", i, registers[i]);
    }

    int opcode = instruction & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int imm = instruction & 0xFFFFF000;

    switch (opcode) {
    case 0x37:
    {
        registers[rd] = imm;
        break;
    }
    case 0x17:
    {
        programCounter -= 4; // Adjust for the default increment
        registers[rd] = imm + programCounter;
        programCounter += 4;
        break;
    }
    default:
        break;
    }
    printf("Executing: %d imm=%d -> rd=%d\n", opcode, imm, rd);

    for (int i = 0; i < 32; i++) {
        printf("Register[%d] = %d\n", i, registers[i]);
    }
}

void static executeInstruction(int instruction) {

    int opcode = instruction & 0x7F;

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
        executeUType(instruction);
        break;
    };
    case 0x6F:
    {
        printf("Instruction J-Type %d\n", instruction);
        break;
    };
    default:
    {
        printf("Unknown Type\n");
        char msg[256];
        snprintf(msg, sizeof(msg), "Unknown operation: %d", opcode);
        programCounter -= 4; // Revert PC increment if the operation is unknown
        throw std::runtime_error(msg);
        break;
    };
    };
}

#endif