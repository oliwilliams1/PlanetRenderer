#include "noise.h"

Noise::Noise() {
	shaderProgram = glCreateProgram();
	if (shaderProgram == 0) {
		std::cout << stderr << "Error creating shader program!" << std::endl;
		exit(1);
	}

	std::string cs;

	if (!ReadFile("shaders/perlin.comp", cs)) exit(1);
	AddShader(shaderProgram, cs.c_str(), GL_COMPUTE_SHADER);

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
	CreateTexture();
	CreateFramebuffer();
	Dispatch();
}

void Noise::Dispatch() {
	glUseProgram(shaderProgram);
	glBindImageTexture(0, noiseTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(32, 32, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Noise::CreateTexture() {
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Noise::CreateFramebuffer() {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, noiseTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Noise::DebugDraw() {
	ImGui::Begin("Perlin noise data view");
	if (ImGui::Button("Generate noise")) Dispatch();
	ImGui::Image((void*)noiseTexture, ImVec2(256, 256));
	ImGui::End();
}

Noise::~Noise() {
	glDeleteProgram(shaderProgram);
}