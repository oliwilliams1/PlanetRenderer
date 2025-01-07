#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

bool WriteFile(const char* pFileName, const std::string& outFile) {
	std::ofstream f(pFileName);

	bool ret = false;

	if (f.is_open()) {
		f << outFile;
		f.close();
		ret = true;
	}
	else {
		std::cout << "Unable to open file for writing: " << pFileName << std::endl;
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

bool LoadBasicModel(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
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

bool LoadAdvancedModel(const std::string& path, std::vector<ObjectData>& objects) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	// Check if the import was successful
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return false;
	}

	// Process all meshes
	objects.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];

		ObjectData objectData;

		// Process vertices, normals, and UVs
		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
			aiVector3D vertex = mesh->mVertices[j];
			objectData.vertices.emplace_back(vertex.x, vertex.y, vertex.z);

			aiVector3D normal = mesh->mNormals[j];
			objectData.normals.emplace_back(normal.x, normal.y, normal.z);

			// Process UVs if available
			if (mesh->mTextureCoords[0]) {
				aiVector3D uv = mesh->mTextureCoords[0][j];
				objectData.uvs.emplace_back(uv.x, uv.y);
			}
			else {
				objectData.uvs.emplace_back(0.0f, 0.0f);
			}
		}

		objectData.indices.reserve(mesh->mNumFaces * 3);

		// Process indices
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];

			// Reverse winding order
			for (unsigned int k = face.mNumIndices; k > 0; --k) {
				objectData.indices.emplace_back(face.mIndices[k - 1]);
			}
		}

		// Retrieve material index for the current mesh
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// Check for a texture path
			aiString texturePath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
				objectData.texturePath = texturePath.C_Str();
			}
		}

		// Add the populated ObjectData to the objects vector
		objects.emplace_back(std::move(objectData));
	}

	return true;
}

void LoadTexture(GLuint* texture, const char* path) {
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	if (data) {
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}
	else {
		std::cerr << "Failed to load texture: " << path << std::endl;
	}
}

void SaveTextureToFile(GLuint texture, const std::string& path, int width, int height, GLenum format) {
	glBindTexture(GL_TEXTURE_2D, texture);

	int channels = (format == GL_RGBA) ? 4 : 3;

	float* floatData = new float[width * height * channels];
	unsigned char* byteData = new unsigned char[width * height * channels];

	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_FLOAT, floatData);

	for (int i = 0; i < width * height * channels; i++) {
		byteData[i] = static_cast<unsigned char>(std::clamp(floatData[i] * 255.0f, 0.0f, 255.0f));
	}

	stbi_write_png((path + ".png").c_str(), width, height, channels, byteData, width * channels);

	delete[] floatData;
	delete[] byteData;

	glBindTexture(GL_TEXTURE_2D, 0);
}