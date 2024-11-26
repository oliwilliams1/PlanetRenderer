#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

class Shader {
public:
    GLuint shaderProgram;
    Shader(const char* vsSource, const char* fsSource, const char* shaderName);
    void use() const;
    const char* shaderName;
    std::string vsSource, fsSource;
};