#pragma once

#include <GLFW/glfw3.h>
#include "CPUInterrupt.h"
#include <stdexcept>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    void init();
    GLFWwindow* getWindow();
    void setResizeCallback(GLFWframebuffersizefun callback);
    void setIcon();
    bool shouldClose();
    void setShouldClose(bool shouldClose);
    void swapBuffers();
    void pollEvents();
    void cleanup();;

private:
    int width;
    int height;
    const char* title;

    GLFWwindow* window;
};
