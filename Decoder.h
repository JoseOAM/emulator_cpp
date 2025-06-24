#pragma once

#include <cstdio>
#include <cassert>

static char* decodeRType(int instruction) {

    int funct3 = (instruction >> 12) & 0x7;
    int funct7 = (instruction >> 25) & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int rs1 = (instruction >> 15) & 0x1F;
    int rs2 = (instruction >> 20) & 0x1F;
    char* formatted_instruction = (char*)malloc(64);

    if (formatted_instruction != NULL) {
        switch (funct3) {
        case 0b000:
        {
            if (funct7 == 0) {
                snprintf(formatted_instruction, 64, "add x%d, x%d, x%d", rd, rs1, rs2);
            }
            else {
                snprintf(formatted_instruction, 64, "sub x%d, x%d, x%d", rd, rs1, rs2);
            }
            break;
        }
        case 0b001:
        {
            snprintf(formatted_instruction, 64, "sll x%d, x%d, x%d", rd, rs1, rs2);
            break;
        }
        case 0b010:
        {
            snprintf(formatted_instruction, 64, "slt x%d, x%d, x%d", rd, rs1, rs2);
            break;
        }
        case 0b011:
        {
            snprintf(formatted_instruction, 64, "sltu x%d, x%d, x%d", rd, rs1, rs2);
            break;
        }
        case 0b100:
        {
            snprintf(formatted_instruction, 64, "xor x%d, x%d, x%d", rd, rs1, rs2);
            break;
        }
        case 0b101:
        {
            if (funct7 == 0) {
                snprintf(formatted_instruction, 64, "srl x%d, x%d, x%d", rd, rs1, rs2);
            }
            else {
                snprintf(formatted_instruction, 64, "sra x%d, x%d, x%d", rd, rs1, rs2);
            }
            break;
        }
        case 0b110:
        {
            snprintf(formatted_instruction, 64, "or x%d, x%d, x%d", rd, rs1, rs2);
            break;
        }
        case 0b111:
        {
            snprintf(formatted_instruction, 64, "and x%d, x%d, x%d", rd, rs1, rs2);
            break;
        }
        default:
        {
            snprintf(formatted_instruction, 64, "Unknown");
            break;
        }
        }
    }
    return formatted_instruction;
}

static char* decodeITypeJumpAndLinkRegister(int instruction) {

    int imm = instruction >> 20;
    int rs1 = (instruction >> 15) & 0x1F;
    int rd = (instruction >> 7) & 0x1F;
    char* formatted_instruction = (char*)malloc(64);

    snprintf(formatted_instruction, 64, "jalr x%d, %d(x%d)", rd, imm, rs1);

    return formatted_instruction;
}

static char* decodeITypeLoad(int instruction) {

    int imm = instruction >> 20;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int rd = (instruction >> 7) & 0x1F;
    char* formatted_instruction = (char*)malloc(64);

    if (formatted_instruction != NULL) {
        switch (funct3) {
        case 0b000:
        {
            snprintf(formatted_instruction, 64, "lb x%d, %d(x%d)", rd, imm, rs1);
            break;
        }
        case 0b001:
        {
            snprintf(formatted_instruction, 64, "lh x%d, %d(x%d)", rd, imm, rs1);
            break;
        }
        case 0b010:
        {
            snprintf(formatted_instruction, 64, "lw x%d, %d(x%d)", rd, imm, rs1);
            break;
        }
        case 0b100:
        {
            snprintf(formatted_instruction, 64, "lbu x%d, %d(x%d)", rd, imm, rs1);
            break;
        }
        case 0b101:
        {
            snprintf(formatted_instruction, 64, "lhu x%d, %d(x%d)", rd, imm, rs1);
            break;
        }
        default:
        {
            snprintf(formatted_instruction, 64, "Unknown");
            break;
        }
        }
    }
    return formatted_instruction;
}

static char* decodeITypeImmediate(int instruction) {

    int imm = instruction >> 20;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int rd = (instruction >> 7) & 0x1F;
    char* formatted_instruction = (char*)malloc(64);

    if (formatted_instruction != NULL) {
        switch (funct3) {
        case 0b000:
        {
            snprintf(formatted_instruction, 64, "addi x%d, x%d, %d", rd, rs1, imm);
            break;
        }
        case 0b010:
        {
            snprintf(formatted_instruction, 64, "slti x%d, x%d, %d", rd, rs1, imm);
            break;
        }
        case 0b011:
        {
            snprintf(formatted_instruction, 64, "sltiu x%d, x%d, %d", rd, rs1, imm);
            break;
        }
        case 0b100:
        {
            snprintf(formatted_instruction, 64, "xori x%d, x%d, %d", rd, rs1, imm);
            break;
        }
        case 0b110:
        {
            snprintf(formatted_instruction, 64, "ori x%d, x%d, %d", rd, rs1, imm);
            break;
        }
        case 0b111:
        {
            snprintf(formatted_instruction, 64, "andi x%d, x%d, %d", rd, rs1, imm);
            break;
        }
        case 0b001:
        {
            imm &= 0x1F;
            snprintf(formatted_instruction, 64, "slli x%d, x%d, %d", rd, rs1, imm);
            break;
        }
        case 0b101:
        {
            if ((imm & 0xFE0) == 0) {
                imm &= 0x1F;
                snprintf(formatted_instruction, 64, "srli x%d, x%d, %d", rd, rs1, imm);
            }
            else {
                imm &= 0x1F;
                snprintf(formatted_instruction, 64, "srai x%d, x%d, %d", rd, rs1, imm);
            }
            break;
        }
        default:
        {
            snprintf(formatted_instruction, 64, "Unknown");
            break;
        }
        }
    }
    return formatted_instruction;
}

static char* decodeITypeControlStatusRegister(int instruction) {
    int csr = instruction >> 20;
    int rs1_or_zimm = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int rd = (instruction >> 7) & 0x1F;
    char* formatted_instruction = (char*)malloc(64);

    if (formatted_instruction != NULL) {
        switch (funct3) {
        case 0b000:
        {
            if (csr == 0) {
                snprintf(formatted_instruction, 64, "ecall");
            }
            else if (csr == 0x01) {
                snprintf(formatted_instruction, 64, "ebreak");
            }
            else if (csr > 0x01) {
                snprintf(formatted_instruction, 64, "mret");
            }
            break;
        }
        case 0b001:
        {
            snprintf(formatted_instruction, 64, "csrrw x%d, %d, x%d", rd, csr, rs1_or_zimm);
            break;
        }
        case 0b010:
        {
            snprintf(formatted_instruction, 64, "csrrs x%d, %d, x%d", rd, csr, rs1_or_zimm);
            break;
        }
        case 0b011:
        {
            snprintf(formatted_instruction, 64, "csrrc x%d, %d, x%d", rd, csr, rs1_or_zimm);
            break;
        }
        case 0b101:
        {
            snprintf(formatted_instruction, 64, "csrrwi x%d, %d, %d", rd, csr, rs1_or_zimm);
            break;
        }
        case 0b110:
        {
            snprintf(formatted_instruction, 64, "csrrsi x%d, %d, %d", rd, csr, rs1_or_zimm);
            break;
        }
        case 0b111:
        {
            snprintf(formatted_instruction, 64, "csrrci x%d, %d, %d", rd, csr, rs1_or_zimm);
            break;
        }
        default:
        {
            snprintf(formatted_instruction, 64, "Unknown");
            break;
        }
        }
    }
    return formatted_instruction;
}

static char* decodeSType(int instruction) {

    int imm11_5 = (instruction >> 25) & 0x7F;
    int rs2 = (instruction >> 20) & 0x1F;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int imm4_0 = (instruction >> 7) & 0x1F;
    int imm = (imm11_5 << 5) | imm4_0;
    char* formatted_instruction = (char*)malloc(64);

    if (formatted_instruction != NULL) {
        switch (funct3) {
        case 0b000:
        {
            snprintf(formatted_instruction, 64, "sb x%d, %d(x%d)", rs2, imm, rs1);
            break;
        }
        case 0b001:
        {
            snprintf(formatted_instruction, 64, "sh x%d, %d(x%d)", rs2, imm, rs1);
            break;
        }
        case 0b010:
        {
            snprintf(formatted_instruction, 64, "sw x%d, %d(x%d)", rs2, imm, rs1);
            break;
        }
        default:
        {
            snprintf(formatted_instruction, 64, "Unknown");
            break;
        }
        }
    }
    return formatted_instruction;
}

static char* decodeBType(int instruction) {
    int imm12 = (instruction >> 31) & 0x1;
    int imm10_5 = (instruction >> 25) & 0x3F;
    int rs2 = (instruction >> 20) & 0x1F;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int imm4_1 = (instruction >> 8) & 0xF;
    int imm11 = (instruction >> 7) & 0x1;
    int imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
    char* formatted_instruction = (char*)malloc(64);

    if (formatted_instruction != NULL) {
        switch (funct3) {
        case 0b000:
        {
            snprintf(formatted_instruction, 64, "beq x%d, x%d, %d", rs1, rs2, imm);
            break;
        }
        case 0b001:
        {
            snprintf(formatted_instruction, 64, "bne x%d, x%d, %d", rs1, rs2, imm);
            break;
        }
        case 0b100:
        {
            snprintf(formatted_instruction, 64, "blt x%d, x%d, %d", rs1, rs2, imm);
            break;
        }
        case 0b101:
        {
            snprintf(formatted_instruction, 64, "bge x%d, x%d, %d", rs1, rs2, imm);
            break;
        }
        case 0b110:
        {
            snprintf(formatted_instruction, 64, "bltu x%d, x%d, %d", rs1, rs2, imm);
            break;
        }
        case 0b111:
        {
            snprintf(formatted_instruction, 64, "bgeu x%d, x%d, %d", rs1, rs2, imm);
            break;
        }
        default:
        {
            snprintf(formatted_instruction, 64, "Unknown");
            break;
        }
        }
    }
    return formatted_instruction;
}

static char* decodeUType(int instruction) {
    int opcode = instruction & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int imm = instruction >> 12;
    char* formatted_instruction = (char*) malloc(64);

    if (formatted_instruction != NULL) {
        switch (opcode) {
        case 0x37:
        {
            snprintf(formatted_instruction, 64, "lui x%d, %d", rd, imm);
            break;
        }
        case 0x17:
        {
            snprintf(formatted_instruction, 64, "auipc x%d, %d", rd, imm);
            break;
        }
        default:
            snprintf(formatted_instruction, 64, "Unknown");
            break;
        }
    }

    return formatted_instruction;
}

static char* decodeJType(int instruction) {
    int imm20 = (instruction >> 31) & 0x1;
    int imm10_1 = (instruction >> 21) & 0x3FF;
    int imm11 = (instruction >> 20) & 0x1;
    int imm19_12 = (instruction >> 12) & 0xFF;

    int opcode = instruction & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);

    char* formatted_instruction = (char*) malloc(64);
    if (formatted_instruction != NULL) {
        snprintf(formatted_instruction, 64, "jal x%d, %d", rd, imm);
    }

    return formatted_instruction;
}

static char* decodeUnknowType(int instruction) {
    char* formatted_instruction = (char*)malloc(64);

    if (formatted_instruction != NULL) {
        snprintf(formatted_instruction, 64, "Unknown Type");
    }

    return formatted_instruction;

}

static char* decodeInstruction(int instruction) {
    int opcode = instruction & 0x7F;
    char* formatted_instruction;

    switch (opcode) {
    case 0x33:
    {
        formatted_instruction = decodeRType(instruction);
        break;
    };
    case 0x67:
    {
        formatted_instruction = decodeITypeJumpAndLinkRegister(instruction);
        break;
    };
    case 0x03:
    {
        formatted_instruction = decodeITypeLoad(instruction);
        break;
    };
    case 0x13:
    {
        formatted_instruction = decodeITypeImmediate(instruction);
        break;
    };
    case 0x73:
    {
        formatted_instruction = decodeITypeControlStatusRegister(instruction);
        break;
    };
    case 0x23:
    {
        formatted_instruction = decodeSType(instruction);
        break;
    };
    case 0x63:
    {
        formatted_instruction = decodeBType(instruction);
        break;
    };
    case 0x37:
    case 0x17:
    {
        formatted_instruction = decodeUType(instruction);
        break;
    };
    case 0x6F:
    {
        formatted_instruction = decodeJType(instruction);
        break;
    };
    default:
    {
        formatted_instruction = decodeUnknowType(instruction);
        break;
    };
    };

    return formatted_instruction;
}

void test_instruction(uint32_t instruction, const char* expected) {
    char* result;
    
    result = decodeInstruction(instruction);
    if (result) {
        printf("Decoded: %s\n", result);
        assert(strcmp(result, expected) == 0);
        free(result);
    }
    else {
        printf("Error: decodeInstruction returned NULL\n");
    }
}
