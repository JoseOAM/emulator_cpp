#include "Main.h"

int main()
{
    const char* path = "E:\\Code\\emulator_cpp\\demos\\paliteiro_demo.txt";
    int fileSize = 0;
    try {
        Memory* memory = new Memory(MEMORY_SIZE);
        FrameBuffer* frameBuffer = new FrameBuffer(FRAME_BUFFER_SIZE);
        Bus* bus = new Bus(frameBuffer, memory);
        ProgramUtils* programUtils = new ProgramUtils(bus);
        GPU* gpu = new GPU(WIDTH, HEIGHT, frameBuffer, "Emulator");
        CPU* cpu = new CPU(bus, memory);
        int* programBin = programUtils->readFile(path, &fileSize);
        /*printf("Program size: %d\n", fileSize);
        for (int i = 0; i < fileSize; i++) {
            print_binary(programBin[i]);
        }
        printf("\n");*/
        programUtils->writeProgramInMemory(programBin, fileSize);
        cpu = new CPU(bus, memory);
        std::thread cpuThread([cpu]() {
            cpu->run();
            });

        std::thread gpuThread([gpu]() {
            gpu->run();
            });

        while (gpuThread.joinable()) {
            if (gpuThread.joinable()) {
                gpu->setShouldClose(true);
                cpu->requestStop();
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (gpuThread.joinable()) gpuThread.join();
        if (cpuThread.joinable()) cpuThread.join();

        delete[] programBin;
        delete gpu;
        delete cpu;
        delete programUtils;
        delete bus;
        delete memory;
        delete frameBuffer;
    }
    catch (const std::exception& e) {
        printf("Error: %s\n", e.what());
    }

    return 0;
}