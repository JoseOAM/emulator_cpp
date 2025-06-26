#include "ShaderProgram.h"

GLuint ShaderProgram::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    checkCompileStatus(shader);
    return shader;
}

void ShaderProgram::checkCompileStatus(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        glDeleteShader(shader);
        throw MemoryException("Shader compile error: %s", infoLog);
    }
}

void ShaderProgram::checkLinkStatus(GLuint program) {
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        char infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        throw MemoryException("Program link error: %s", infoLog);
    }
}

void ShaderProgram::loadShaders() {
    const char* vertexSource =
        "#version 460\n"
        "layout (location = 0) in vec2 vertexPosition;\n"
        "layout (location = 1) in vec4 vertexColor;\n"
        "layout (location = 2) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    gl_Position = vec4(vertexPosition, 0.0, 1.0);\n"
        "    TexCoord = texCoord;\n"
        "    outColor = vertexColor;\n"
        "}\n";

    const char* fragmentSource =
        "#version 460\n"
        "in vec2 TexCoord;\n"
        "in vec4 outColor;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D ourTexture;\n"
        "void main() {\n"
        "    FragColor = texture(ourTexture, TexCoord) * outColor;\n"
        "}\n";

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);
    checkLinkStatus(programId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::use() const {
    glUseProgram(programId);
}

void ShaderProgram::cleanup() {
    if (programId != 0) {
        glDeleteProgram(programId);
        programId = 0;
    }
}

ShaderProgram::~ShaderProgram() {
    cleanup();
}
