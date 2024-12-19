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

GLuint CompileComputeShader(const char* source) {
    GLuint shader = glCreateProgram();
    if (shader == 0) {
        std::cout << stderr << "Error creating shader program!" << std::endl;
        exit(1);
    }

    std::string cs;

    if (!ReadFile(source, cs)) exit(1);
    AddShader(shader, cs.c_str(), GL_COMPUTE_SHADER);

    GLint success = 0;
    GLchar errorLog[1024] = { 0 };

    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shader, sizeof(errorLog), nullptr, errorLog);
        std::cout << "Error linking shader program: " << errorLog << std::endl;
        exit(1);
    }

    glUseProgram(shader);

    return shader;
}

GLuint GetUniformLocation(GLuint shaderProgram, const char* uniformName) {
    GLuint location = glGetUniformLocation(shaderProgram, uniformName);
    if (location == -1) {
        std::cerr << "Warning: " << uniformName << " uniform not found!" << std::endl;
    }
    return location;
}

bool LoadModel(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs);

    // Check if the import was successful
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // Process all meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // Process vertices
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            aiVector3D vertex = mesh->mVertices[j];
            vertices.emplace_back(vertex.x, vertex.y, vertex.z);
        }

        // Process indices
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                indices.push_back(face.mIndices[k]);
            }
        }
    }

    return true;
}