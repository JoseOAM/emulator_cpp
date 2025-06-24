#include "CPU.h"

CPU::CPU(Bus bus) {
    this->registers = new int[32]();
    this->csrRegisters = new int[4096]();
    initializeRegisters();
    this->bus = bus;
}

CPU::~CPU() {
    free(this->registers);
    free(this->csrRegisters);
}

void CPU::initializeRegisters() {
    this->registers[2] = Memory.getMemorySize() - 4;
    this->registers[3] = Memory.getMemorySize() / 2;
    this->registers[8] = this->registers[2];
}

void CPU::setCsrRegister(int index, int value) {
    this->csrRegisters[index] = value;
}

void CPU::setStartTime() {
    startTime = time(NULL);
}

int CPU::signExtendImmediate(int immediate, int bitWidth) {
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

void CPU::executeRType(int instruction) {

    int funct3 = (instruction >> 12) & 0x7;
    int funct7 = (instruction >> 25) & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int rs1 = (instruction >> 15) & 0x1F;
    int rs2 = (instruction >> 20) & 0x1F;

    int value1 = this->registers[rs1];
    int value2 = this->registers[rs2];
    int result;

    switch (funct3) {
    case 0b000:
    {
        if (funct7 == 0) {
            result = rs1 + rs2;
        }
        else {
            result = rs1 - rs2;
        }
        break;
    }
    case 0b001:
    {
        result = rs1 << rs2;
        break;
    }
    case 0b010:
    {
        result = rs1 < rs2 ? 1 : 0;
        break;
    }
    case 0b011:
    {
        result = ((uint32_t)rs1 < (uint32_t)rs2) ? 1 : 0;
        break;
    }
    case 0b100:
    {
        result = rs1 ^ rs2;
        break;
    }
    case 0b101:
    {
        if (funct7 == 0) {
            result = ((uint32_t)rs1) >> rs2;
        }
        else {
            result = rs1 >> rs2;
        }
        break;
    }
    case 0b110:
    {
        result = rs1 | rs2;
        break;
    }
    case 0b111:
    {
        result = rs1 & rs2;
        break;
    }
    default:
    {
        result = 0;
        break;
    }
    }

    this->registers[rd] = result;
}

void CPU::executeITypeJumpAndLinkRegister(int instruction) {

    int imm = instruction >> 20;
    int rs1 = (instruction >> 15) & 0x1F;
    int rd = (instruction >> 7) & 0x1F;
    int signedImm = signExtendImmediate(imm, 12);
    this->registers[rd] = programCounter;
    programCounter = (this->registers[rs1] + signedImm) & ~1;
}

void CPU::executeITypeLoad(int instruction) {

    int imm = instruction >> 20;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int rd = (instruction >> 7) & 0x1F;
    int signedImm = signExtendImmediate(imm, 12);
    int address = this->registers[rs1] + signedImm;

    if (address < 0) {
        return;
    }

    int value = this->bus.read(address, address + 4)[0];

    switch (funct3) {
    case 0b000:
    {
        this->registers[rd] = (int8_t)value;
        break;
    }
    case 0b001:
    {
        this->registers[rd] = (int16_t)value;
        break;
    }
    case 0b010:
    {
        this->registers[rd] = value;
        break;
    }
    case 0b100:
    {
        this->registers[rd] = value & 0xFF;
        break;
    }
    case 0b101:
    {
        this->registers[rd] = value & 0xFFFF;
        break;
    }
    default:
    {
        break;
    }
    }
}

void CPU::executeITypeImmediate(int instruction) {

    int imm = instruction >> 20;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int rd = (instruction >> 7) & 0x1F;
    int signedImm = signExtendImmediate(imm, 12);
    int result = 0;

    switch (funct3) {
    case 0b000:
    {
        result = this->registers[rs1] + signedImm;
        break;
    }
    case 0b010:
    {
        result = (this->registers[rs1] < signedImm) ? 1 : 0;
        break;
    }
    case 0b011:
    {
        result = ((uint32_t)this->registers[rs1] < (uint32_t)signedImm) ? 1 : 0;
        break;
    }
    case 0b100:
    {
        result = this->registers[rs1] ^ signedImm;
        break;
    }
    case 0b110:
    {
        result = this->registers[rs1] | signedImm;
        break;
    }
    case 0b111:
    {
        result = this->registers[rs1] & signedImm;
        break;
    }
    case 0b001:
    {
        result = this->registers[rs1] << signedImm;
        break;
    }
    case 0b101:
    {
        if ((imm & 0xFE0) == 0) {
            result = ((uint32_t)this->registers[rs1]) >> signedImm;
        }
        else {
            result = this->registers[rs1] >> signedImm;
        }
        break;
    }
    default:
    {
        result = 0;
        break;
    }
    }

    this->registers[rd] = result;
}

void CPU::executeITypeControlStatusRegister(int instruction) {
    int csr = instruction >> 20;
    int rs1OrZimm = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int rd = (instruction >> 7) & 0x1F;
    int signedRs1OrZimm = signExtendImmediate(rs1OrZimm, 12);
    int csrValue = this->csrRegisters[csr];

    switch (funct3) {
    case 0b000:
    {
        if (csr == 0) {
            executeEcallType();
        }
        else if (csr == 0x01) {
            executeEbreakType();
        }
        else if (csr > 0x01) {
            executeMretType();
        }
        break;
    }
    case 0b001:
    {
        this->csrRegisters[csr] = this->registers[signedRs1OrZimm];
        this->registers[rd] = csrValue;
        break;
    }
    case 0b010:
    {
        this->csrRegisters[csr] |= this->registers[signedRs1OrZimm];
        this->registers[rd] = csrValue;
        break;
    }
    case 0b011:
    {
        this->csrRegisters[csr] &= ~this->registers[signedRs1OrZimm];
        this->registers[rd] = csrValue;
        break;
    }
    case 0b101:
    {
        this->csrRegisters[csr] = signedRs1OrZimm;
        this->registers[rd] = csrValue;
        break;
    }
    case 0b110:
    {
        this->csrRegisters[csr] |= signedRs1OrZimm;
        this->registers[rd] = csrValue;
        break;
    }
    case 0b111:
    {
        this->csrRegisters[csr] &= ~signedRs1OrZimm;
        this->registers[rd] = csrValue;
        break;
    }
    default:
    {
        break;
    }
    }
}

void CPU::executeSType(int instruction) {

    int imm11_5 = (instruction >> 25) & 0x7F;
    int rs2 = (instruction >> 20) & 0x1F;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int imm4_0 = (instruction >> 7) & 0x1F;
    int imm = (imm11_5 << 5) | imm4_0;
    int signedImm = signExtendImmediate(imm, 12);
    int address = this->registers[rs1] + signedImm;
    if (address < 0) {
        throw MemoryException("Memory access out of bounds: %d", address);
    }

    switch (funct3) {
    case 0b000:
    {
        uint8_t value = this->registers[rs2] & 0xFF;
        this->bus.write(address, &value, 1);
        break;
    }
    case 0b001:
    {
        uint16_t value = this->registers[rs2] & 0xFFFF;
        this->bus.write(address, (uint8_t*)&value, 2);
        break;
    }
    case 0b010:
    {
        uint32_t value = this->registers[rs2];
        this->bus.write(address, (uint8_t*)&value, 4);
        break;
    }
    default:
    {
        break;
    }
    }
}

void CPU::executeBType(int instruction) {
    int imm12 = (instruction >> 31) & 0x1;
    int imm10_5 = (instruction >> 25) & 0x3F;
    int rs2 = (instruction >> 20) & 0x1F;
    int rs1 = (instruction >> 15) & 0x1F;
    int funct3 = (instruction >> 12) & 0x7;
    int imm4_1 = (instruction >> 8) & 0xF;
    int imm11 = (instruction >> 7) & 0x1;
    int imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
    bool condition;
    int signedImm = signExtendImmediate(imm, 12);

    switch (funct3) {
    case 0b000:
    {
        condition = this->registers[rs1] == this->registers[rs2];
        break;
    }
    case 0b001:
    {
        condition = this->registers[rs1] != this->registers[rs2];
        break;
    }
    case 0b100:
    {
        condition = this->registers[rs1] < this->registers[rs2];
        break;
    }
    case 0b101:
    {
        condition = this->registers[rs1] >= this->registers[rs2];
        break;
    }
    case 0b110:
    {
        condition = (uint32_t)this->registers[rs1] < (uint32_t)this->registers[rs2];
        break;
    }
    case 0b111:
    {
        condition = (uint32_t)this->registers[rs1] >= (uint32_t)this->registers[rs2];
        break;
    }
    default:
    {
        condition = false;
        break;
    }
    }
    if (condition) {
        programCounter += signedImm - 4;
    }
}


void CPU::executeUType(int instruction) {

    int opcode = instruction & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int imm = (instruction >> 12) << 12;

    switch (opcode) {
    case 0x37:
    {
        this->registers[rd] = imm;
        break;
    }
    case 0x17:
    {
        programCounter -= 4;
        this->registers[rd] = imm + programCounter;
        programCounter += 4;
        break;
    }
    default:
        break;
    }
}

void CPU::executeJType(int instruction) {
    int imm20 = (instruction >> 31) & 0x1;
    int imm10_1 = (instruction >> 21) & 0x3FF;
    int imm11 = (instruction >> 20) & 0x1;
    int imm19_12 = (instruction >> 12) & 0xFF;
    int opcode = instruction & 0x7F;
    int rd = (instruction >> 7) & 0x1F;
    int imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
    int signedImm = signExtendImmediate(imm, 20);

    this->registers[rd] = programCounter;
    programCounter += signedImm - 4;
}

void CPU::executeEcallType() {
    throw MemoryException("Program has terminated via syscall exit.");
}

void CPU::executeEbreakType() {
    int timerInterruptCount = this->bus.read(1024, 1028)[0];
    int keyInterruptCount = this->bus.read(1028, 1032)[0];

    throw MemoryException("Program has terminated via syscall exit.");
}

void CPU::executeMretType() {
    programCounter = this->csrRegisters[MEPC];
    setCsrRegister(MIP, 0);
    setStartTime();
}

void CPU::executeInstruction(int instruction) {
    int opcode = instruction & 0x7F;

    switch (opcode) {
    case 0x33:
    {
        executeRType(instruction);
        break;
    };
    case 0x67:
    {
        executeITypeJumpAndLinkRegister(instruction);
        break;
    };
    case 0x03:
    {
        executeITypeLoad(instruction);
        break;
    };
    case 0x13:
    {
        executeITypeImmediate(instruction);
        break;
    };
    case 0x73:
    {
        executeITypeControlStatusRegister(instruction);
        break;
    };
    case 0x23:
    {
        executeSType(instruction);
        break;
    };
    case 0x63:
    {
        executeBType(instruction);
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
        executeJType(instruction);
        break;
    };
    default:
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "Unknown operation: %d", opcode);
        throw std::runtime_error(msg);
        break;
    };
    };
}