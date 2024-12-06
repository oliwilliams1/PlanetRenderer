#include "shader.h"

bool Shader::ReadFile(const char* pFileName, std::string& outFile) {
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

void Shader::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
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

Shader::Shader(const char* vsSource, const char* fsSource, const char* shaderName) {
    this->shaderName = shaderName;
    this->vsSource = vsSource;
    this->fsSource = fsSource;

    LoadBasicShaders();
}

void Shader::LoadBasicShaders() {
    shaderProgram = glCreateProgram();
    if (shaderProgram == 0) {
        std::cout << stderr << "Error creating shader program!" << std::endl;
        exit(1);
    }

    std::string vs, fs;

    if (!ReadFile(vsSource.c_str(), vs)) exit(1);
    AddShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!ReadFile(fsSource.c_str(), fs)) exit(1);
    AddShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint success = 0;
    GLchar errorLog[1024] = { 0 };

    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shaderProgram, sizeof(errorLog), nullptr, errorLog);
        std::cout << "Error linking shader program: " << errorLog << std::endl;
        exit(1);
    }

    glUseProgram(shaderProgram);
}

Shader::~Shader() {
    glDeleteProgram(shaderProgram);
}

void Shader::Reload() {
    LoadBasicShaders();
}

void Shader::use() const {
	glUseProgram(shaderProgram);
}