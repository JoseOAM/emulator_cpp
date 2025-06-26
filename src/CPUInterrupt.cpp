#include "CPUInterrupt.h"

CPUInterrupt::CPUInterrupt()
    : isInterruptEnabled(false),
    interruptData(0),
    keyPressedFlag(false),
    startTime(std::chrono::steady_clock::now())
{
}

int CPUInterrupt::checkInterruption(int clockSpeedMillis) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

    if (elapsed >= clockSpeedMillis) {
        isInterruptEnabled = false;
        setStartTime();
        return 1;
    }
    else if (keyPressedFlag.load()) {
        isInterruptEnabled = false;
        keyPressedFlag = false;
        return interruptData + 2;
    }
    return 0;
}

void CPUInterrupt::setStartTime() {
    startTime = std::chrono::steady_clock::now();
}

bool CPUInterrupt::isInterrupted() const {
    return stopRequested.load();
}

void CPUInterrupt::requestStop() {
    stopRequested = true;
}
