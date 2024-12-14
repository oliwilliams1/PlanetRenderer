#include "noise.h"

Noise::Noise() {
	this->sampleOffsetSize = 6.0f;
	this->needToDispatch   = false;

	cubemapNoiseShaderProgram = CompileComputeShader("shaders/cubemapNoise.comp");

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
	// Cubemap noise generation
	glUseProgram(cubemapNoiseShaderProgram);

	glBindImageTexture(0, cubemapNoiseTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(32, 32, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	needToDispatch = false;
	lastDispatchTime = glfwGetTime();
}

void Noise::CreateTextures() {
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void Noise::CreateFramebuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboCubemapNoise);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCubemapNoise);

	for (int i = 0; i < 6; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapNoiseTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Error: cubemap framebuffer is not complete!" << std::endl;
		}
	}
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

Noise::~Noise() {
	glDeleteTextures(1, &cubemapNoiseTexture);
	glDeleteFramebuffers(1, &fboCubemapNoise);

	glDeleteProgram(cubemapNoiseShaderProgram);
}