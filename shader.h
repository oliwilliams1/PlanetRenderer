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
    ~Shader();
    void LoadBasicShaders();
    virtual void Reload();
    void use() const;
    const char* shaderName;
    std::string vsSource, fsSource;

protected:
    bool ReadFile(const char* pFileName, std::string& outFile);
    void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
};