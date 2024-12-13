#include "noise.h"

Noise::Noise() {
	this->seed             = 0;
	this->scale            = 1.0f;
	this->octaves          = 4;
	this->persistence      = 0.25f;
	this->sampleOffsetSize = 6.0f;
	this->needToDispatch   = false;

	noiseShaderProgram = CompileComputeShader("shaders/terrainGen.comp");

	seedLocation = glGetUniformLocation(noiseShaderProgram, "u_Seed");
	if (seedLocation == -1) {
		std::cerr << "Warning: u_Seed uniform not found!" << std::endl;
	}

	scaleLocation = glGetUniformLocation(noiseShaderProgram, "u_Scale");
	if (scaleLocation == -1) {
		std::cerr << "Warning: scale uniform not found!" << std::endl;
	}

	octavesLocation = glGetUniformLocation(noiseShaderProgram, "u_Octaves");
	if (octavesLocation == -1) {
		std::cerr << "Warning: octaves uniform not found!" << std::endl;
	}

	persistenceLocation = glGetUniformLocation(noiseShaderProgram, "u_Persistence");
	if (persistenceLocation == -1) {
		std::cerr << "Warning: persistence uniform not found!" << std::endl;
	}

	normalShaderProgram = CompileComputeShader("shaders/terrainNormal.comp");

	normal_TerrainHeightmapLocation = glGetUniformLocation(normalShaderProgram, "u_TerrainHeightmap");
	if (normal_TerrainHeightmapLocation == -1) {
		std::cerr << "Warning: u_TerrainHeightmap uniform not found!" << std::endl;
	}

	normal_SampleOffsetSizeLocation = glGetUniformLocation(normalShaderProgram, "u_NormalOffsetSize");
	if (normal_SampleOffsetSizeLocation == -1) {
		std::cerr << "Warning: u_NormalOffsetSize uniform not found!" << std::endl;
	}

	CreateTextures();
	CreateFramebuffers();
	Dispatch();
}

GLuint Noise::CompileComputeShader(const char* source) {
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

void Noise::Dispatch() {
	// Calculate terrain heightmap
	glUseProgram(noiseShaderProgram);

	glUniform1i(seedLocation, seed);
	glUniform1i(octavesLocation, octaves);
	glUniform1f(scaleLocation, scale);
	glUniform1f(persistenceLocation, persistence);

	glBindFramebuffer(GL_FRAMEBUFFER, fboNoise);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, noiseTexture, 0);

	glBindImageTexture(0, noiseTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(128, 64, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Calculate normals from heightmap
	glUseProgram(normalShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glUniform1i(normal_TerrainHeightmapLocation, 0);
	glUniform1f(normal_SampleOffsetSizeLocation, sampleOffsetSize);

	glBindFramebuffer(GL_FRAMEBUFFER, fboNormal);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalTexture, 0);
	
	glBindImageTexture(0, normalTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(128, 64, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	needToDispatch = false;
	lastDispatchTime = glfwGetTime();
}

void Noise::CreateTextures() {
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 1024, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &normalTexture);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 1024, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Noise::CreateFramebuffers() {
	glGenFramebuffers(1, &fboNoise);
	glBindFramebuffer(GL_FRAMEBUFFER, fboNoise);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, noiseTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: noise framebuffer is not complete!" << std::endl;
	}

	glGenFramebuffers(1, &fboNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboNormal);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: normal framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Noise::DebugDraw() {
	ImGui::Begin("Perlin noise data view");
	ImGui::Image((ImTextureID)(intptr_t)noiseTexture, ImVec2(384, 192));
	ImGui::Image((ImTextureID)(intptr_t)normalTexture, ImVec2(384, 192));
	if (ImGui::SliderInt("Seed", &seed, 0, 1000)) needToDispatch = true;
	if (ImGui::SliderInt("Octaves", &octaves, 1, 20)) needToDispatch = true;
	if (ImGui::SliderFloat("Scale", &scale, 1.0f, 10.0f)) needToDispatch = true;
	if (ImGui::SliderFloat("Persistence", &persistence, 0.0f, 1.0f)) needToDispatch = true;
	if (ImGui::SliderFloat("Sample offset px", &sampleOffsetSize, 0.1f, 20.0f)) needToDispatch = true;
	ImGui::End();

	if (needToDispatch) {
		float currentTime = glfwGetTime();
		if (currentTime - lastDispatchTime > 0.2f) {
			Dispatch();
		}
	}
}

Noise::~Noise() {
	glDeleteTextures(1, &noiseTexture);
	glDeleteTextures(1, &normalTexture);
	glDeleteFramebuffers(1, &fboNoise);
	glDeleteFramebuffers(1, &fboNormal);

	glDeleteProgram(noiseShaderProgram);
	glDeleteProgram(normalShaderProgram);
}