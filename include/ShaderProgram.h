#pragma once

#include <glad/gl.h>
#include "MemoryException.h"
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>

class ShaderProgram {
private:
    GLuint programId = 0;

    GLuint compileShader(GLenum type, const char* source);
    void checkCompileStatus(GLuint shader);
    void checkLinkStatus(GLuint program);

public:
    ShaderProgram() = default;
    ~ShaderProgram();

    void loadShaders();
    void use() const;
    void cleanup();
    GLuint getId() const { return programId; }
};
