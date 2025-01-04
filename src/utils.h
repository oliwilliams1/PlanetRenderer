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

bool LoadBasicModel(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
bool LoadAdvancedModel(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices);
bool ReadFile(const char* pFileName, std::string& outFile);
bool WriteFile(const char* pFileName, const std::string& inFile);
void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
GLuint CompileComputeShader(const char* source);
GLuint GetUniformLocation(GLuint shaderProgram, const char* uniformName);
void LoadTexture(GLuint* texture, const char* path);