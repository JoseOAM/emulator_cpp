#pragma once

#include <stdexcept>
#include <ctime>
#include "CPU.h"
#include "Bus.h"
#include "CPUInterrupt.h"

#define MIE 772
#define MTVEC 773
#define MEPC 833
#define MCAUSE 834
#define MTVAL 835
#define MIP 836
#define CLOCK 1000

class CPU : public CPUInterrupt {
public:
    CPU(Bus* bus, Memory* memory);
    ~CPU();

    void run();
    void processNextInstruction();
    void setCsrRegister(int index, int value);
    void executeInstruction(int instruction);

private:
    long long startTime;
    int* registers;
    int* csrRegisters;
    int programCounter;

    Bus* bus;
    Memory* memory;
    //GUI* gui;

    void interruptHandler();
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
