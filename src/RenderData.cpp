#include "RenderData.h"

RenderData::RenderData(int w, int h, FrameBuffer* frameBuffer)
    : width(w),
    height(h),
    bufferSize(frameBuffer->getFrameBufferSize()),
    numVertices(bufferSize / 8),
    vao(0),
    vbo(0),
    textureId(0),
    pboIds(nullptr),
    pboCount(2),
    nextPboIndex(0),
    shouldStop(false)
{
}

RenderData::~RenderData() {
    if (pboIds != nullptr) {
        delete[] pboIds;
        pboIds = nullptr;
    }
}

void RenderData::setup() {
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    setupTexture();
    setupVAOAndVBO();
    setupPBOs();

    glPointSize(4.0f);
}

void RenderData::setupTexture() {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
}

void RenderData::setupVAOAndVBO() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int stride = 8 * sizeof(float);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void RenderData::setupPBOs() {
    const int pboCount = 2;
    this->pboIds = new GLuint[pboCount];

    glGenBuffers(pboCount, this->pboIds);
    for (int i = 0; i < pboCount; ++i) {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, this->pboIds[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, this->bufferSize, nullptr, GL_STREAM_DRAW);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void RenderData::draw(RenderDataDto dataDto) {
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, this->bufferSize, dataDto.vertex, GL_STREAM_DRAW);

    GLuint pboId = this->pboIds[this->nextPboIndex];
    this->nextPboIndex = (this->nextPboIndex + 1) % 2;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, this->bufferSize, dataDto.pixel, GL_STREAM_DRAW);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->width, this->height,
        GL_RGBA, GL_FLOAT, 0);

    glGenerateMipmap(GL_TEXTURE_2D);

    glDrawArrays(GL_POINTS, 0, this->numVertices);
}

void RenderData::cleanup() {
    if (this->pboIds != nullptr) {
        glDeleteBuffers(2, this->pboIds);
        delete[] this->pboIds;
        this->pboIds = nullptr;
    }

    glDeleteBuffers(1, &this->vbo);
    glDeleteVertexArrays(1, &this->vao);
    glDeleteTextures(1, &this->textureId);
}
