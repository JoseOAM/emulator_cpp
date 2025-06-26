#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Window::Window(int width, int height, const char* title)
    : width(width), height(height), title(title) {
    window = NULL;
}

Window::~Window() {
    //if (window) {
    //    glfwDestroyWindow(window);
    //    window = nullptr;
    //}
    //glfwTerminate();
}

void Window::init() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwShowWindow(window);

    const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (vidMode) {
        glfwSetWindowPos(
            window,
            (vidMode->width - width) / 2,
            (vidMode->height - height) / 2
        );
    }

    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        CPUInterrupt cpuInterrupt;
        if (cpuInterrupt.isInterruptEnabled && action == GLFW_PRESS) {
            cpuInterrupt.keyPressedFlag = true;
            cpuInterrupt.interruptData = key;
        }
        });
}

GLFWwindow* Window::getWindow() {
    return window;
}

void Window::setResizeCallback(GLFWframebuffersizefun callback) {
    glfwSetFramebufferSizeCallback(window, callback);
}

void Window::setIcon() {
    int w, h, comp;
    unsigned char* iconPixels = stbi_load("../assets/icon.png", &w, &h, &comp, 4);
    GLFWimage icon;

    if (iconPixels) {
        icon.width = w;
        icon.height = h;
        icon.pixels = iconPixels;
    }
    else {
        static unsigned char emptyIconPixels[4] = { 0,0,0,0 };
        icon.width = 1;
        icon.height = 1;
        icon.pixels = emptyIconPixels;
    }

    glfwSetWindowIcon(window, 1, &icon);

    if (iconPixels) {
        stbi_image_free(iconPixels);
    }
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window) == GLFW_TRUE;
}

void Window::setShouldClose(bool shouldClose) {
    glfwSetWindowShouldClose(window, shouldClose ? GLFW_TRUE : GLFW_FALSE);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::cleanup() {
    glfwDestroyWindow(window);
}
