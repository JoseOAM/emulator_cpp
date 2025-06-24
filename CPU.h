#pragma once

#include <stdio.h>
#include <cstdint>
#include <stdexcept>
#include <time.h>
#include "MemoryException.h"

class CPU {
public:
    CPU(Bus bus);
    ~CPU();

    void setCsrRegister(int index, int value);
    void setStartTime();

    void executeInstruction(int instruction);

private:
    long long startTime;
    int MIE = 772;
    int MTVEC = 773;
    int MEPC = 833;
    int MCAUSE = 834;
    int MTVAL = 835;
    int MIP = 836;

    int* registers;
    int* csrRegisters;
    int programCounter;

    Bus* bus;
    GUI* gui;

    void initializeRegisters();
    int signExtendImmediate(int immediate, int bitWidth);
    void executeRType(int instruction);
    void executeITypeJumpAndLinkRegister(int instruction);
    void executeITypeLoad(int instruction);
    void executeITypeImmediate(int instruction);
    void executeITypeControlStatusRegister(int instruction);
    void executeSType(int instruction);
    void executeBType(int instruction);
    void executeUType(int instruction);
    void executeJType(int instruction);

    void executeEcallType();
    void executeEbreakType();
    void executeMretType();
};
