#include "GPU.h"

GPU::GPU(int w, int h, FrameBuffer* fb, const char* t)
    : RenderData(w, h, fb), frameBuffer(fb), title(t) {
    this->width = w;
    this->height = h;
    this->window = new Window(w, h, t);
    this->shaderProgram = new ShaderProgram();
}

int GPU::getWidth() const {
    return width;
}

int GPU::getHeight() const {
    return height;
}

const char* GPU::getTitle() const {
    return title;
}

void GPU::run() {
    init();
    while (isRunning()) {
        try {
            render();
        }
        catch (const MemoryException& e) {
            throw std::runtime_error(e.what());
        }
    }
    cleanup();
}

void GPU::init() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    window->init();
    window->setIcon();
    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize OpenGL context with glad.");
    }
    glViewport(0, 0, width, height);

    window->setResizeCallback([](GLFWwindow*, int newWidth, int newHeight) {
        glViewport(0, 0, newWidth, newHeight);
        });

    shaderProgram->loadShaders();
    shaderProgram->use();

    setup();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

bool GPU::isRunning() {
    return !window->shouldClose();
}

void GPU::setShouldClose(bool shouldClose) {
    window->setShouldClose(shouldClose);
}

void GPU::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw(frameBuffer->getRenderData());
    window->swapBuffers();
    window->pollEvents();
}

void GPU::cleanup() {
    RenderData::cleanup();
    if (shaderProgram) {
        shaderProgram->cleanup();
        delete shaderProgram;
        shaderProgram = nullptr;
    }
    if (window) {
        window->cleanup();
        delete window;
        window = nullptr;
    }
}
