#include "utils.h"

bool ReadFile(const char* pFileName, std::string& outFile) {
    std::ifstream f(pFileName);

    bool ret = false;

    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            outFile.append(line);
            outFile.append("\n");
        }

        f.close();
        ret = true;
    }
    else {
        std::cout << "Unable to open file: " << pFileName << std::endl;
    }

    return ret;
}

void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        std::cout << stderr << "Error creating shader type %d\n" << ShaderType;
        exit(1);
    }

    const GLchar* p[1];
    p[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0] = (GLint)strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[4096];
        glGetShaderInfoLog(ShaderObj, 4096, NULL, InfoLog);
        std::cout << stderr << "Error compiling shader:\n" << pShaderText << std::endl << InfoLog;
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint GetUniformLocation(GLuint shaderProgram, const char* uniformName) {
    GLuint location = glGetUniformLocation(shaderProgram, uniformName);
    if (location == -1) {
        std::cerr << "Warning: " << uniformName << " uniform not found!" << std::endl;
    }
    return location;
}
