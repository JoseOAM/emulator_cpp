#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <thread>
#include "FrameBuffer.h"

class RenderData {
public:
    RenderData(int width, int height, FrameBuffer* frameBuffer);
    virtual ~RenderData();

    virtual void run() = 0;

protected:
    const int width;
    const int height;
    const int bufferSize;
    const int numVertices;

    GLuint vao;
    GLuint vbo;
    GLuint textureId;

    GLuint* pboIds;
    int pboCount;
    int nextPboIndex;

    std::atomic<bool> shouldStop;

    void setup();
    void setupTexture();
    void setupVAOAndVBO();
    void setupPBOs();
    void draw(RenderDataDto dataDto);
    void cleanup();
};
