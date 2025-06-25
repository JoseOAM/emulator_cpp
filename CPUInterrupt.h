#pragma once

#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>

class CPUInterrupt {
public:
    CPUInterrupt();
    virtual ~CPUInterrupt() = default;

    int checkInterruption(int clockSpeedMillis);
    void setStartTime();
    bool isInterrupted() const;

    std::atomic<bool> isInterruptEnabled;
    std::atomic<int> interruptData;
    std::atomic<bool> keyPressedFlag;

    void requestStop();
private:
    std::chrono::steady_clock::time_point startTime;
    std::atomic<bool> stopRequested;
};
