#pragma once
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

bool ReadFile(const char* pFileName, std::string& outFile);
void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
GLuint GetUniformLocation(GLuint shaderProgram, const char* uniformName);