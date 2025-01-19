#include "atmosphereRenderer.h"

Atmosphere::Atmosphere(float width, float height, GLuint gFragPos, GLuint gDepth) {
	this->width  = width;
	this->height = height;
	this->gDepth = gDepth;
	this->gFragPos = gFragPos;

	this->shader = new Shader("shaders/atmosphere.vert", "shaders/atmosphere.frag", "Atmosphere");
	SetupQuad();
	CreateTextures();
	
	shader->use();
	glUniform1i(GetUniformLocation(shader->shaderProgram, "u_FragPos"), 1);
	glUniform1i(GetUniformLocation(shader->shaderProgram, "u_Depth"), 2);
}

void Atmosphere::SetupQuad() {
	float vertices[18] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Atmosphere::CreateTextures() {
	glGenFramebuffers(1, &gAtmosphereFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gAtmosphereFBO);

	glGenTextures(1, &gAtmosphere);
	glBindTexture(GL_TEXTURE_2D, gAtmosphere);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAtmosphere, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Atmosphere::Draw() {
	glBindFramebuffer(GL_FRAMEBUFFER, gAtmosphereFBO);
	glViewport(0, 0, width, height);

	shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gAtmosphere);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gFragPos);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Atmosphere::DebugDraw() {
	ImGui::Text("Atmosphere");
	float aspectRatio = (float)width / (float)height;
	int width = 175;
	int height = (int)(width / aspectRatio);
	bool flipUv = false;

	ImVec2 uv0 = flipUv ? ImVec2(0.0f, 0.0f) : ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = flipUv ? ImVec2(1.0f, 1.0f) : ImVec2(1.0f, 0.0f);

	ImGui::Image((ImTextureID)(intptr_t)gFragPos, ImVec2(width, height), uv0, uv1);
	ImGui::SameLine();
	ImGui::Image((ImTextureID)(intptr_t)gAtmosphere, ImVec2(width, height), uv0, uv1);
}

Atmosphere::~Atmosphere() {
	glDeleteFramebuffers(1, &gAtmosphereFBO);
	glDeleteTextures(1, &gAtmosphere);
}