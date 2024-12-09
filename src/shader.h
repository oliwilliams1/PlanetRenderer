#pragma once

#include "utils.h"

class Shader {
public:
    GLuint shaderProgram;
    Shader(const char* vsSource, const char* fsSource, const char* shaderName);
    ~Shader();
    void LoadBasicShaders();
    virtual void Reload();
    void use() const;
    const char* shaderName;
    std::string vsSource, fsSource;
};