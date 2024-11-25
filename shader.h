#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>

class Shader {
public:
    GLuint shaderProgram;
    Shader(const char* vsSource, const char* fsSource);
    void use() const;
};