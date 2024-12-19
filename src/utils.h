#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/Scene.h>
#include <assimp/PostProcess.h>
#include <glm/glm.hpp>

bool LoadModel(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
bool ReadFile(const char* pFileName, std::string& outFile);
void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
GLuint CompileComputeShader(const char* source);
GLuint GetUniformLocation(GLuint shaderProgram, const char* uniformName);