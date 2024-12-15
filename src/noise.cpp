#include "noise.h"

Noise::Noise() {
	this->sampleOffsetSize = 6.0f;
	this->needToDispatch   = false;

	cubemapNoiseShaderProgram = CompileComputeShader("shaders/cubemapNoise.comp");
	cubemapNormalShaderProgram = CompileComputeShader("shaders/cubemapNormal.comp");

	normal_NoiseSamplerLocation = GetUniformLocation(cubemapNormalShaderProgram, "u_TerrainCubemap");	

	CreateTextures();
	CreateFramebuffers();
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
	// Cubemap noise generation
	glUseProgram(cubemapNoiseShaderProgram);
	glBindImageTexture(0, cubemapNoiseTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(32, 32, 6);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Cubemap normal generation
	glUseProgram(cubemapNormalShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapNoiseTexture);
	glUniform1i(normal_NoiseSamplerLocation, 0);

	glBindImageTexture(0, cubemapNormalTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glUniform1i(normal_NoiseSamplerLocation, 0);

	glDispatchCompute(32, 32, 6);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Ending
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	needToDispatch = false;
	lastDispatchTime = glfwGetTime();
}

void Noise::CreateTextures() {
	// Noise texture
	glGenTextures(1, &cubemapNoiseTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapNoiseTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, nullptr);
	}

	// Normal map texture
	glGenTextures(1, &cubemapNormalTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapNormalTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, nullptr);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Noise::CreateFramebuffers() {
	// Noise cubemap framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboCubemapNoise);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCubemapNoise);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubemapNoiseTexture, 0);

	// Normal cubemap framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboCubemapNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCubemapNormal);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubemapNormalTexture, 0);

	// Bind defualt
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Noise::DebugDraw() {
	ImGui::Begin("Perlin noise data view");
	if (ImGui::SliderFloat("Sample offset px", &sampleOffsetSize, 0.1f, 20.0f)) needToDispatch = true;
	ImGui::End();

	if (needToDispatch) {
		float currentTime = glfwGetTime();
		if (currentTime - lastDispatchTime > 0.2f) {
			Dispatch();
		}
	}
}

// Destroy
Noise::~Noise() {
	glDeleteTextures(1, &cubemapNoiseTexture);
	glDeleteTextures(1, &cubemapNormalTexture);

	glDeleteFramebuffers(1, &fboCubemapNoise);
	glDeleteFramebuffers(1, &fboCubemapNormal);

	glDeleteProgram(cubemapNoiseShaderProgram);
	glDeleteProgram(cubemapNormalShaderProgram);
}