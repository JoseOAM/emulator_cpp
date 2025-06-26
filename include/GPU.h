#pragma once

#include <stdexcept>
#include "ShaderProgram.h"
#include "Window.h"
#include "RenderData.h"
#include "FrameBuffer.h"
#include "MemoryException.h"
#include <GLFW/glfw3.h>

class GPU : public RenderData {
public:
    GPU(int width, int height, FrameBuffer* frameBuffer, const char* title);
    virtual ~GPU() = default;

    void run() override;
    void setShouldClose(bool shouldClose);
    int getWidth() const;
    int getHeight() const;
    const char* getTitle() const;

    void cleanup();

private:
    int width;
    int height;

    const char* title;
    FrameBuffer* frameBuffer;
    ShaderProgram* shaderProgram;
    Window* window;

    void init();
    bool isRunning();
    void render();
};
