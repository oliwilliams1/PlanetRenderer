#include "noise.h"

Noise::Noise() {
	this->seed        = 0;
	this->scale       = 5.0f;
	this->octaves     = 6;
	this->persistence = 0.5f;
	this->needToDispatch = false;

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

	seedLocation = glGetUniformLocation(shaderProgram, "seed");
	if (seedLocation == -1) {
		std::cerr << "Warning: seed uniform not found!" << std::endl;
	}

	scaleLocation = glGetUniformLocation(shaderProgram, "scale");
	if (scaleLocation == -1) {
		std::cerr << "Warning: scale uniform not found!" << std::endl;
	}

	octavesLocation = glGetUniformLocation(shaderProgram, "octaves");
	if (octavesLocation == -1) {
		std::cerr << "Warning: octaves uniform not found!" << std::endl;
	}

	persistenceLocation = glGetUniformLocation(shaderProgram, "persistence");
	if (persistenceLocation == -1) {
		std::cerr << "Warning: persistence uniform not found!" << std::endl;
	}

	CreateTexture();
	CreateFramebuffer();
	Dispatch();
}

void Noise::Dispatch() {
	glUseProgram(shaderProgram);

	glUniform1i(seedLocation, seed);
	glUniform1i(octavesLocation, octaves);
	glUniform1f(scaleLocation, scale);
	glUniform1f(persistenceLocation, persistence);

	glBindImageTexture(0, noiseTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(128, 64, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	needToDispatch = false;
	lastDispatchTime = glfwGetTime();
}

void Noise::CreateTexture() {
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 1024, 0, GL_RGBA, GL_FLOAT, nullptr);
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
	ImGui::Image((void*)noiseTexture, ImVec2(512, 256));
	if (ImGui::SliderInt("Seed", &seed, 0, 1000)) needToDispatch = true;
	if (ImGui::SliderInt("Octaves", &octaves, 1, 10)) needToDispatch = true;
	if (ImGui::SliderFloat("Scale", &scale, 1.0f, 10.0f)) needToDispatch = true;
	if (ImGui::SliderFloat("Persistence", &persistence, 0.0f, 1.0f)) needToDispatch = true;
	ImGui::End();

	if (needToDispatch) {
		float currentTime = glfwGetTime();
		if (currentTime - lastDispatchTime > 0.2f) {
			Dispatch();
		}
	}
}

Noise::~Noise() {
	glDeleteProgram(shaderProgram);
}